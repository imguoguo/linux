// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <drm/drm_print.h>
#include <drm/drm_atomic.h>
#include <drm/drm_blend.h>
#include <drm/drm_atomic_uapi.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_drv.h>
#include <drm/drm_managed.h>
#include <drm/drm_plane.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_color_mgmt.h>
#include <drm/drm_gem.h>
#include <linux/component.h>
#include <linux/dma-mapping.h>
#include "spacemit_cmdlist.h"
#include "spacemit_dmmu.h"
#include "spacemit_drm.h"
#include "dpu/dpu_saturn.h"
#include "dpu/dpu_trace.h"

/* The AFBC decoder exists but no modifier has been validated on hardware. */
static const u64 supported_format_modifiers[] = {
	DRM_FORMAT_MOD_LINEAR,
	DRM_FORMAT_MOD_INVALID
};

struct spacemit_plane *to_spacemit_plane(struct drm_plane *plane)
{
	return container_of(plane, struct spacemit_plane, plane);
}

static int spacemit_plane_check_rdma(const struct spacemit_hw_rdma *rdma, u32 rdma_id,
				     struct drm_plane_state *state)
{
	unsigned int rot = state->rotation;
	unsigned int zpos = state->zpos;
	u32 format = state->fb->format->format;
	u16 hw_formats = rdma[rdma_id].formats;
	u16 hw_rots = rdma[rdma_id].rots;
	const struct drm_format_info *info = drm_format_info(format);
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(state->crtc);

	trace_spacemit_plane_check_rdma(a_crtc->dev_id);
	if (info->is_yuv && (hw_formats & FORMAT_RAW_YUV) == 0) {
		drm_dbg(state->plane->dev, "rdma%d doesn't support RAW YUV format with zpos%d!\n",
			rdma_id, zpos);
		return -EINVAL;
	}

	if (is_rot_90_270(rot)) {
		if ((hw_rots & ROTATE_RAW_90_270) == 0) {
			drm_dbg(state->plane->dev,
				"rdma%d doesn't support 90/270 rotation with zpos%d!\n",
				rdma_id, zpos);
			return -EINVAL;
		}
	} else {
		if ((hw_rots & ROTATE_COMMON) == 0) {
			drm_dbg(state->plane->dev,
				"rdma%d doesn't support common rotation with zpos%d!\n", rdma_id,
				zpos);
			return -EINVAL;
		}
	}

	return 0;
}

static int spacemit_plane_atomic_check(struct drm_plane *plane,
				       struct drm_atomic_commit *atomic_state)
{
	struct drm_plane_state *state = drm_atomic_get_new_plane_state(atomic_state, plane);
	struct drm_framebuffer *fb = state->fb;
	u32 src_w, src_h, src_x, src_y, scl_in_width, scl_in_height;
	u32 crtc_w, crtc_h;
	struct spacemit_plane_state *cur_state = to_spacemit_plane_state(state);
	u32 cur_rdma_id = cur_state->rdma_id;
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	const struct spacemit_hw_rdma *rdmas = hwdev->rdmas;
	struct spacemit_crtc *a_crtc = NULL;

	struct drm_crtc_state *crtc_state = NULL;
	int ret;

	if (state->crtc)
		crtc_state = drm_atomic_get_new_crtc_state(atomic_state,
							   state->crtc);

	ret = drm_atomic_helper_check_plane_state(state, crtc_state,
						  DRM_PLANE_NO_SCALING / SPACEMIT_MAX_SCALE_FACTOR,
						  DRM_PLANE_NO_SCALING * SPACEMIT_MAX_SCALE_FACTOR,
						  true, false);
	if (ret)
		return ret;

	if (!state->visible)
		return 0;

	a_crtc = to_spacemit_crtc(state->crtc);
	trace_spacemit_plane_atomic_check(a_crtc->dev_id);

	src_x = state->src_x >> 16;
	src_y = state->src_y >> 16;
	src_w = state->src_w >> 16;
	src_h = state->src_h >> 16;
	crtc_w = state->crtc_w;
	crtc_h = state->crtc_h;

	/* The programming paths use the raw rectangles, so refuse to clip. */
	if ((state->src.x1 >> 16) != src_x || (state->src.y1 >> 16) != src_y ||
	    (drm_rect_width(&state->src) >> 16) != src_w ||
	    (drm_rect_height(&state->src) >> 16) != src_h ||
	    state->dst.x1 != state->crtc_x || state->dst.y1 != state->crtc_y ||
	    drm_rect_width(&state->dst) != crtc_w ||
	    drm_rect_height(&state->dst) != crtc_h) {
		drm_dbg_kms(plane->dev, "clipped plane rectangles not supported\n");
		return -EINVAL;
	}

	/* For multi planes, only support planes with its offsets is set */
	if (fb->format->num_planes > 3 ||
	    (fb->format->num_planes > 2 && fb->offsets[2] == 0) ||
	    (fb->format->num_planes > 1 && fb->offsets[1] == 0)) {
		drm_dbg_kms(plane->dev, "%s, Unsupported plane format: plane_num:%d offsets[1]:%d offsets[2]:%d\n",
			    __func__, fb->format->num_planes, fb->offsets[1], fb->offsets[2]);
		return -EINVAL;
	}

	if (fb->format->format == DRM_FORMAT_NV12) {
		if (src_x % 2 || src_y % 2 || src_w % 2 || src_h % 2) {
			drm_dbg_kms(plane->dev, "YUV420 coordinations must be even! src_x:%d src_y:%d src_w:%d src_h:%d\n",
				    src_x, src_y, src_w, src_h);
			return -EINVAL;
		}
	}

	/* One-dimensional sources are invalid (only 0x0 = constant fill) */
	if ((src_w == 0) != (src_h == 0)) {
		drm_dbg_kms(plane->dev, "%s, Unsupported src_w %d src_h %d\n",
			    __func__, src_w, src_h);
		return -EINVAL;
	}

	/* adjust rdma id */
	if (src_w == 0 && src_h == 0)
		cur_rdma_id = RDMA_INVALID_ID;	/* constant-fill layer, no RDMA */
	else if (cur_rdma_id == RDMA_INVALID_ID)
		cur_rdma_id = state->zpos;	/* first commit: bind channel by zpos */
	cur_state->rdma_id = cur_rdma_id;

	/* Skip solid color */
	if (cur_rdma_id != RDMA_INVALID_ID) {
		if (is_rot_90_270(state->rotation)) {
			scl_in_width  = src_h;
			scl_in_height = src_w;
		} else {
			scl_in_width  = src_w;
			scl_in_height = src_h;
		}

		if (scl_in_width != crtc_w || scl_in_height != crtc_h) {
			if ((scl_in_width * SPACEMIT_MAX_SCALE_FACTOR < crtc_w) ||
			    (crtc_w * SPACEMIT_MAX_SCALE_FACTOR < scl_in_width)) {
				drm_dbg_kms(plane->dev, "Width scaling exceeds %dx limit: %d -> %d\n",
					    SPACEMIT_MAX_SCALE_FACTOR, scl_in_width, crtc_w);
				return -EINVAL;
			}
			if ((scl_in_height * SPACEMIT_MAX_SCALE_FACTOR < crtc_h) ||
			    (crtc_h * SPACEMIT_MAX_SCALE_FACTOR < scl_in_height)) {
				drm_dbg_kms(plane->dev, "Height scaling exceeds %dx limit: %d -> %d\n",
					    SPACEMIT_MAX_SCALE_FACTOR, scl_in_height, crtc_h);
				return -EINVAL;
			}
			cur_state->use_scl = true;
		} else {
			cur_state->use_scl = false;
		}
		if (cur_rdma_id < hwdev->rdma_nums) {
			if (spacemit_plane_check_rdma(rdmas, cur_rdma_id, state))
				return -EINVAL;
		} else {
			drm_dbg_kms(plane->dev, "Invalid rdma id:%d\n", cur_rdma_id);
			return -EINVAL;
		}

		if (a_crtc->core->cal_layer_fbcmem_size(plane, state)) {
			drm_dbg_kms(plane->dev, "plane:%d Invalid fbcmem size\n", state->zpos);
			return -EINVAL;
		}
	}

	if (cur_rdma_id != RDMA_INVALID_ID) {
		cur_state->mmu_tbl.size =
			((PAGE_ALIGN(fb->obj[0]->size) >> PAGE_SHIFT) +
			 HW_ALIGN_TTB_NUM) * 4;
		cur_state->mmu_tbl.va =
			dma_alloc_coherent(a_crtc->dev, cur_state->mmu_tbl.size,
					   &cur_state->mmu_tbl.pa,
					   GFP_KERNEL | __GFP_ZERO);
		if (!cur_state->mmu_tbl.va)
			return -ENOMEM;
	}

	/* The commit cannot fail, so take the DMA buffer while -ENOMEM is an answer. */
	ret = spacemit_cmdlist_alloc(a_crtc->dev, &cur_state->cl[cur_state->cur_cl]);
	if (ret)
		return ret;

	cur_state->format = spacemit_plane_hw_get_format_id(fb->format->format);
	if (cur_state->format == SPACEMIT_DPU_INVALID_FORMAT_ID) {
		drm_dbg_kms(plane->dev, "Can't support format:%p4cc\n", &fb->format->format);
		return -EINVAL;
	}

	/* Use default values as they are not actually used now  */
	cur_state->right_image = 0;
	return 0;
}

static void spacemit_plane_atomic_update(struct drm_plane *plane,
					 struct drm_atomic_commit *state)
{
	int ret = 0;
	struct drm_plane_state *old_state = drm_atomic_get_old_plane_state(state, plane);
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(plane->state->crtc);
	struct spacemit_plane_state *spacemit_pstate = to_spacemit_plane_state(plane->state);
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u32 rdma_id = spacemit_pstate->rdma_id;
	struct drm_crtc_state *crtc_state;
	struct spacemit_crtc_state *spacemit_crtc_state;
	struct cmdlist *cl = NULL;
	int idx;

	if (!drm_dev_enter(plane->dev, &idx))
		return;

	trace_spacemit_plane_atomic_update(a_crtc->dev_id);

	spacemit_pstate->cur_cl = 0;

	crtc_state = a_crtc->crtc.state;
	spacemit_crtc_state = to_spacemit_crtc_state(crtc_state);
	spacemit_crtc_state->cur_cl = 0;

	hwdev->get_cl_rdma_buf(a_crtc);
	/* cl_rdma free is at the end of the function and early return */

	hwdev->update_csc_matrix(plane, old_state);

	hwdev->plane_update_hw_channel(plane, DPU_NO_SLICE);

	/* No need for solid color layer (mmu_tbl preallocated in check) */
	if (rdma_id < hwdev->rdma_nums && spacemit_pstate->cur_cl == 0 &&
	    spacemit_pstate->mmu_tbl.va) {
		u8 tbu_id = !spacemit_pstate->right_image ? (rdma_id * 2) : (rdma_id * 2 + 1);

		ret = spacemit_dmmu_map(plane->state->fb, &spacemit_pstate->mmu_tbl, tbu_id,
					a_crtc->cl_rdma, plane);
		if (!ret)
			cmdlist_regs_packing(plane_to_cl(plane), CMDLIST_MOD_RDMA, a_crtc->cl_rdma);
		else
			drm_err(plane->dev, "%s failed to map plane with ret = %d\n", __func__,
				ret);
	}

	cl = &spacemit_pstate->cl[spacemit_pstate->cur_cl];
	cl->rch_start_cmps_y = CMDLIST_SENTINEL_POISON;
	cl->cmdlist_ch_y_other = CMDLIST_SENTINEL_POISON;
	free_cmdlist_regs(a_crtc->cl_rdma);

	drm_dev_exit(idx);
}

static void spacemit_plane_atomic_disable(struct drm_plane *plane,
					  struct drm_atomic_commit *state)
{
	struct drm_plane_state *old_state = drm_atomic_get_old_plane_state(state, plane);
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	int idx;

	if (!drm_dev_enter(plane->dev, &idx))
		return;

	spacemit_dmmu_unmap(plane);
	hwdev->plane_disable_hw_channel(plane, old_state);

	drm_dev_exit(idx);
}

static void spacemit_plane_reset(struct drm_plane *plane)
{
	struct spacemit_plane *p = to_spacemit_plane(plane);
	struct spacemit_plane_state *s;
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	if (plane->state) {
		s = to_spacemit_plane_state(plane->state);
		__drm_atomic_helper_plane_destroy_state(plane->state);
		kfree(s);
		plane->state = NULL;
	}

	s = kzalloc(sizeof(*s), GFP_KERNEL);
	if (s) {
		__drm_atomic_helper_plane_reset(plane, &s->state);
		s->state.zpos = hwdev->plane_nums - p->hw_pid - 1;
		s->rdma_id = RDMA_INVALID_ID;
		s->scaler_id = SCALER_INVALID_ID;
	}
}

static struct drm_plane_state *
spacemit_plane_atomic_duplicate_state(struct drm_plane *plane)
{
	struct spacemit_plane_state *s;
	struct spacemit_plane_state *old_state = to_spacemit_plane_state(plane->state);
	struct spacemit_crtc *a_crtc = NULL;
	u8 i = 0;

	if (plane->crtc) {
		a_crtc = to_spacemit_crtc(plane->crtc);
		trace_spacemit_plane_atomic_duplicate_state(a_crtc->dev_id);
	}

	s = kzalloc(sizeof(*s), GFP_KERNEL);
	if (!s)
		return NULL;

	__drm_atomic_helper_plane_duplicate_state(plane, &s->state);

	WARN_ON(s->state.plane != plane);

	for (i = 0; i < MAX_CL_NUM; i++) {
		s->cl[i].index = i;
		s->cl[i].type = CMDLIST_PLANE;
	}

	s->rdma_id = old_state->rdma_id;
	s->format = old_state->format;
	s->right_image = old_state->right_image;
	s->scaler_id = SCALER_INVALID_ID;
	s->use_scl = false;
	s->fbcmem_size = 0;

	return &s->state;
}

static void spacemit_plane_atomic_destroy_state(struct drm_plane *plane,
						struct drm_plane_state *state)
{
	int i = 0;
	struct spacemit_plane_state *spacemit_pstate = to_spacemit_plane_state(state);
	struct spacemit_crtc *a_crtc = NULL;

	if (state->crtc) {
		a_crtc = to_spacemit_crtc(state->crtc);

		if (spacemit_pstate->mmu_tbl.va)
			dma_free_coherent(a_crtc->dev, spacemit_pstate->mmu_tbl.size,
					  spacemit_pstate->mmu_tbl.va, spacemit_pstate->mmu_tbl.pa);

		for (i = 0; i < MAX_CL_NUM; i++) {
			if (spacemit_pstate->cl[i].va)
				dma_free_coherent(a_crtc->dev, spacemit_pstate->cl[i].size,
						  spacemit_pstate->cl[i].va,
						  spacemit_pstate->cl[i].pa);
		}
		trace_spacemit_plane_atomic_destroy_state(a_crtc->dev_id);
	}
	__drm_atomic_helper_plane_destroy_state(state);

	kfree(to_spacemit_plane_state(state));
}

static int spacemit_plane_create_properties(struct spacemit_plane *p, int index)
{
	unsigned int support_modes = BIT(DRM_MODE_BLEND_PIXEL_NONE) |
		BIT(DRM_MODE_BLEND_PREMULTI) |
		BIT(DRM_MODE_BLEND_COVERAGE);
	int ret = 0;

	/* 90/270 needs the AFBC path, which is not exposed. */
	drm_plane_create_rotation_property(&p->plane,
					   DRM_MODE_ROTATE_0,
					   DRM_MODE_ROTATE_0 |
					   DRM_MODE_ROTATE_180 |
					   DRM_MODE_REFLECT_MASK);

	/* create zpos property */
	drm_plane_create_zpos_immutable_property(&p->plane, index);

	/* create layer alpha property */
	drm_plane_create_alpha_property(&p->plane);

	/* create blend mode property */
	drm_plane_create_blend_mode_property(&p->plane, support_modes);

	ret = drm_plane_create_color_properties(&p->plane,
						BIT(DRM_COLOR_YCBCR_BT601) |
					BIT(DRM_COLOR_YCBCR_BT709) |
					BIT(DRM_COLOR_YCBCR_BT2020),
					BIT(DRM_COLOR_YCBCR_LIMITED_RANGE) |
					BIT(DRM_COLOR_YCBCR_FULL_RANGE),
					DRM_COLOR_YCBCR_BT601, DRM_COLOR_YCBCR_LIMITED_RANGE);
	if (ret)
		drm_err(p->plane.dev, "Failed to create color properties %d\n", ret);

	return 0;
}

static const struct drm_plane_helper_funcs spacemit_plane_helper_funcs = {
	.atomic_check = spacemit_plane_atomic_check,
	.atomic_update = spacemit_plane_atomic_update,
	.atomic_disable = spacemit_plane_atomic_disable,
};

static const struct drm_plane_funcs spacemit_plane_funcs = {
	.update_plane = drm_atomic_helper_update_plane,
	.disable_plane	= drm_atomic_helper_disable_plane,
	.destroy = drm_plane_cleanup,
	.reset = spacemit_plane_reset,
	.atomic_duplicate_state = spacemit_plane_atomic_duplicate_state,
	.atomic_destroy_state = spacemit_plane_atomic_destroy_state,
};

struct drm_plane *spacemit_plane_init(struct drm_device *drm,
				      struct spacemit_crtc *a_crtc)
{
	struct drm_plane *primary = NULL;
	struct spacemit_plane *p = NULL;
	enum drm_plane_type plane_type;
	int err, i, j;
	u32 *formats;
	struct spacemit_drm_private *priv = drm->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u8 n_planes = hwdev->plane_nums;
	u8 n_formats = hwdev->n_formats;
	u8 n_fbcmems = hwdev->n_fbcmems;
	u8 n_rdmas = hwdev->rdma_nums;
	u32 plane_crtc_mask;

	trace_spacemit_plane_init(a_crtc->dev_id);
	if (n_fbcmems * 2 != n_rdmas) {
		drm_err(drm, "Unmatched rdma and fbcmem numbers, n_rdmas:%d n_fbcmems:%d!\n",
			n_rdmas, n_fbcmems);
		err = -EINVAL;
		return ERR_PTR(err);
	}

	formats = kcalloc(n_formats, sizeof(*formats), GFP_KERNEL);
	if (!formats) {
		err = -ENOMEM;
		return ERR_PTR(err);
	}

	/* Create planes for CRTC(a_crtc->dev_id) */
	/* The CRTC is created right after this, so its index is the current count. */
	plane_crtc_mask = 1 << drm->mode_config.num_crtc;

	for (i = 0; i < n_planes; i++) {
		/* drmm, not devm: an open DRM fd outlives the platform unbind. */
		p = drmm_kzalloc(drm, sizeof(*p), GFP_KERNEL);
		if (!p) {
			kfree(formats);
			return ERR_PTR(-ENOMEM);
		}

		/* build the list of DRM supported formats based on the map */
		for (j = 0; j < n_formats; j++)
			formats[j] = hwdev->formats[j].format;

		plane_type = (i == 0)
			   ? DRM_PLANE_TYPE_PRIMARY
			   : DRM_PLANE_TYPE_OVERLAY;

		err = drm_universal_plane_init(drm, &p->plane, plane_crtc_mask,
					       &spacemit_plane_funcs, formats,
					       n_formats,  supported_format_modifiers,
					       plane_type, NULL);
		if (err) {
			drm_err(drm, "fail to init %s plane%d\n", (i == 0) ? "primary" : "overlay",
				i);
			kfree(formats);
			return ERR_PTR(err);
		}

		drm_plane_helper_add(&p->plane, &spacemit_plane_helper_funcs);

		spacemit_plane_create_properties(p, i);

		p->hwdev = hwdev;
		p->hw_pid = n_planes - i - 1;
		if (i == 0)
			primary = &p->plane;
	}

	kfree(formats);

	if (!primary)
		return ERR_PTR(-ENODEV);

	return primary;
}
