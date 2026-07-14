// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <linux/stddef.h>
#include <linux/export.h>
#include <drm/drm_atomic.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_mode.h>
#include <drm/drm_print.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_fourcc.h>
#include "../spacemit_cmdlist.h"
#include "saturn_fbcmem.h"

int get_raw_data_plane_rdma_mem_size(u32 drm_4cc_fmt, bool rot_90_or_270, u32 plane_crop_width,
				     u32 *output_mem_size)
{
	u8 index = 0;
	u32 ret_mem_size = 0;
	u32 data_plane_mem_size[3] = {0}; /* max 3 plane, YUV data */
	const struct drm_format_info *info = NULL;

	if (rot_90_or_270) {
		/* dpu hardware request 32K fix size, 32 byte unit has considered */
		ret_mem_size = 1024;
	} else {
		info = drm_format_info(drm_4cc_fmt);
		if (info->cpp[0] == 0) {
			pr_debug("FBC_MEM: not support format %p4cc\n", &drm_4cc_fmt);
			return -EINVAL;
		}
		if (info->num_planes == 1 && !info->is_yuv) {
			data_plane_mem_size[0] = plane_crop_width * info->cpp[0];
		} else if (info->num_planes >= 2 && info->is_yuv) {
			data_plane_mem_size[0] = plane_crop_width * info->cpp[0];
			data_plane_mem_size[1] = plane_crop_width * info->cpp[1] / info->hsub;
			if (info->num_planes == 3)
				data_plane_mem_size[2] = plane_crop_width *
							 info->cpp[2] / info->vsub;
		} else {
			pr_debug("FBC_MEM: not considered drm format %p4cc\n", &drm_4cc_fmt);
			return -EINVAL;
		}
		for (index = 0; index < info->num_planes; index++) { /* max 3 plane, YUV data */
			data_plane_mem_size[index] = roundup(data_plane_mem_size[index],
							     64); /* dpu hardware request */
			ret_mem_size += data_plane_mem_size[index];
		}
		ret_mem_size = roundup(ret_mem_size, 64);
		ret_mem_size = ret_mem_size / 32;
	}
	if (output_mem_size) {
		*output_mem_size = ret_mem_size;
		pr_debug("FBC_MEM: raw layer, fmt = %p4cc, rot_90_270 = %d, crop_w = %u, cal memsize = %u\n",
			 &drm_4cc_fmt, rot_90_or_270, plane_crop_width, *output_mem_size);
	}
	return 0;
}

int saturn_cal_layer_fbcmem_size(struct drm_plane *plane,
				 struct drm_plane_state *state)
{
	struct spacemit_plane_state *pstate = to_spacemit_plane_state(state);
	u32 drm_4cc_fmt = pstate->state.fb->format->format;
	bool rot_90_or_270 = is_rot_90_270(pstate->state.rotation);
	u32 crop_w = pstate->state.src_w >> 16;
	int ret;

	/* Only linear sizing: the compressed path is not exposed. */
	ret = get_raw_data_plane_rdma_mem_size(drm_4cc_fmt, rot_90_or_270, crop_w,
					       &pstate->fbcmem_size);
	if (ret < 0)
		drm_err(plane->dev, "FBC_MEM: failed to size plane %ux%u+%u+%u\n",
			pstate->state.src_w >> 16, pstate->state.src_h >> 16,
			pstate->state.src_x >> 16, pstate->state.src_y >> 16);

	return ret;
}

/* Even RDMA (0, 2, ...): sole owner of the front of its fbc memory bank. */
static int saturn_fbcmem_place_even(struct spacemit_hw_device *hwdev,
				    struct spacemit_crtc_rdma *rdmas,
				    u32 *fbc_mems_left, u8 index)
{
	u32 cur_rdma_fbcmem_size = rdmas[index].fbcmem.size;
	u32 pri_fbcmem_size = hwdev->fbcmem_sizes[index / 2] / FBCMEM_UNIT;

	pr_debug("rdma%u, cur_rdma_fbcmem_size = %u, pri_fbcmem_size = %u\n",
		 index, cur_rdma_fbcmem_size, pri_fbcmem_size);
	if (cur_rdma_fbcmem_size > pri_fbcmem_size) {
		pr_debug("FBC_MEM: rdma %d use %d byte, excess the size %d\n",
			 index, cur_rdma_fbcmem_size, pri_fbcmem_size);
		return -ENOSPC;
	}
	/* The preceding odd RDMA may not already share this bank. */
	if (index > 0 && rdmas[index - 1].fbcmem.map) {
		pr_info_once("FBC_MEM: both rdma %d %d use fbc memory\n",
			     index, index - 1);
		return -ENOSPC;
	}
	rdmas[index].fbcmem.start = 0;
	rdmas[index].fbcmem.map = true;
	fbc_mems_left[index / 2] -= cur_rdma_fbcmem_size;
	pr_debug("FBC_MEM: rdma: id = %d, size = %u, start = %u, map = %u, fbc_mems_left[%u] = %d\n",
		 index, cur_rdma_fbcmem_size, rdmas[index].fbcmem.start,
		 rdmas[index].fbcmem.map, index / 2, fbc_mems_left[index / 2]);

	return 0;
}

/* Odd RDMA takes what is left of its own bank, spilling into the next. */
static int saturn_fbcmem_place_odd(struct spacemit_hw_device *hwdev,
				   struct spacemit_crtc_rdma *rdmas,
				   u32 *fbc_mems_left, u8 index)
{
	u32 cur_rdma_fbcmem_size = rdmas[index].fbcmem.size;
	u32 pri_fbcmem_size = hwdev->fbcmem_sizes[index / 2] / FBCMEM_UNIT;
	u8 rdma_nums = hwdev->rdma_nums;
	struct spacemit_crtc_fbcmem *fbcmem;
	u8 sec_fbcmem_index;
	u32 sec_fbcmem_size;

	if (cur_rdma_fbcmem_size <= fbc_mems_left[index / 2]) { /* not share fbc mem */
		pr_debug("rdma%u, cur_rdma_fbcmem_size = %u, fbc_mems_left[%u] = %u\n",
			 index, cur_rdma_fbcmem_size, index / 2, fbc_mems_left[index / 2]);
		rdmas[index].fbcmem.map = false;
		rdmas[index].fbcmem.start = pri_fbcmem_size - fbc_mems_left[index / 2];
		rdmas[index].fbcmem.size = fbc_mems_left[index / 2]; /* use all the mem left */
		fbc_mems_left[index / 2] = 0;
		pr_debug("FBC_MEM: rdma: id = %d, size = %u, actually size = %u, start = %u, map = %u, fbc_mems_left[%u] = 0\n",
			 index, cur_rdma_fbcmem_size, rdmas[index].fbcmem.size,
			 rdmas[index].fbcmem.start, rdmas[index].fbcmem.map, index / 2);
		return 0;
	}

	/* Need to share the next bank's fbc mem. */
	sec_fbcmem_index = (index / 2 + 1) % (rdma_nums / 2);
	sec_fbcmem_size = hwdev->fbcmem_sizes[sec_fbcmem_index] / FBCMEM_UNIT;
	pr_debug("rdma%u, cur_rdma_fbcmem_size = %u, fbc_mems_left[%u] = %d, sec_fbcmem_size = %u\n",
		 index, cur_rdma_fbcmem_size, index / 2, fbc_mems_left[index / 2],
		 sec_fbcmem_size);
	if (cur_rdma_fbcmem_size > fbc_mems_left[index / 2] + sec_fbcmem_size) {
		pr_debug("FBC_MEM: rdma %d use %d mem size, but left %d\n",
			 index, cur_rdma_fbcmem_size,
		       fbc_mems_left[index / 2] + sec_fbcmem_size);
		return -ENOSPC;
	}
	if (index == rdma_nums - 1) { /* last rdma id */
		if (fbc_mems_left[0] != hwdev->fbcmem_sizes[0] / FBCMEM_UNIT) { /* fbc mem0 has used */
			pr_debug("FBC_MEM: rdma %d can not use fbc mem 0 for it has been used\n",
				 index);
			return -ENOSPC;
		}
		rdmas[index].fbcmem.size = fbc_mems_left[index / 2] + sec_fbcmem_size;
		cur_rdma_fbcmem_size = rdmas[index].fbcmem.size;
	}
	fbc_mems_left[sec_fbcmem_index] -= (cur_rdma_fbcmem_size - fbc_mems_left[index / 2]);
	pr_debug("rmda%u, fbc_mems_left[%u] = %d, fbc_mems_left[%u] = %u\n",
		 index, index / 2, fbc_mems_left[index / 2], sec_fbcmem_index,
		 fbc_mems_left[sec_fbcmem_index]);
	rdmas[index].fbcmem.start = pri_fbcmem_size - fbc_mems_left[index / 2];
	rdmas[index].fbcmem.map = true;
	fbc_mems_left[index / 2] = 0;

	fbcmem = &rdmas[index].fbcmem;
	pr_debug("FBC_MEM: rdma: id = %d, actually size = %u, start = %u, map = %u\n",
		 index, fbcmem->size, fbcmem->start, fbcmem->map);

	return 0;
}

int saturn_adjust_rdma_fbcmem(struct spacemit_hw_device *hwdev,
			      struct spacemit_crtc_rdma *rdmas)
{
	int ret = -ENOSPC;
	u8 index = 0;
	u32 *fbc_mems_left = NULL;
	u8 rdma_nums = hwdev->rdma_nums;
	struct spacemit_crtc_fbcmem *fbcmem = NULL;

	for (index = 0; index < rdma_nums; index++) {
		fbcmem = &rdmas[index].fbcmem;
		pr_debug("input rdmas[%u/%u]: mode = %d, start = %d, size = %d, map = %d\n",
			 index, rdma_nums, rdmas[index].mode, fbcmem->start,
			 fbcmem->size, fbcmem->map);
	}

	fbc_mems_left = kzalloc_objs(*fbc_mems_left, rdma_nums / 2);
	if (!fbc_mems_left)
		goto free;
	for (index = 0; index < rdma_nums / 2; index++) {
		fbc_mems_left[index] = hwdev->fbcmem_sizes[index] / FBCMEM_UNIT;
		pr_debug("fbcmem_sizes[%u/%u] = %u, total fbc_mems_left = %u\n",
			 index, rdma_nums / 2, hwdev->fbcmem_sizes[index], fbc_mems_left[index]);
		if (fbc_mems_left[index] == 0) {
			pr_err("FBC_MEM: error fbcmem_sizes[%d] = %u\n", index,
			       hwdev->fbcmem_sizes[index]);
			goto free;
		}
	}

	for (index = 0; index < rdma_nums; index++) {
		int err;

		if (rdmas[index].fbcmem.size == 0) {
			pr_debug("return directly for rmda %u fbcmem size is 0\n", index);
			continue;
		}

		if (index % 2 == 0) /* dma_id is even: 0, 2, 4, 6... */
			err = saturn_fbcmem_place_even(hwdev, rdmas, fbc_mems_left, index);
		else /* odd rdma: rdma 1, 3, 5... */
			err = saturn_fbcmem_place_odd(hwdev, rdmas, fbc_mems_left, index);
		if (err)
			goto free;
	}
	ret = 0;

free:
	kfree(fbc_mems_left);

	return ret;
}
