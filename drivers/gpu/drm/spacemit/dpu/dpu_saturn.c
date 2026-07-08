// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <linux/delay.h>
#include <linux/dma-mapping.h>
#include <linux/wait.h>
#include <linux/workqueue.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/mfd/syscon.h>
#include <linux/pm_runtime.h>
#include <linux/pm_qos.h>
#include <linux/regmap.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/types.h>
#include <linux/math64.h>
#include <drm/drm_atomic.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_gem.h>
#include "dpu_saturn.h"
#include "saturn_fbcmem.h"
#include "../spacemit_cmdlist.h"
#include "../spacemit_dmmu.h"
#include "../spacemit_dpu_reg.h"
#include "../spacemit_drm.h"
#include <video/display_timing.h>
#include <dt-bindings/display/spacemit_dpu.h>
#include "saturn_regs/ops_hee.h"

#define CREATE_TRACE_POINTS
#include "dpu_trace.h"

enum {
	DITHER_MODE_ARM = 0,
	DITHER_MODE_PATTERN,
	DITHER_MODE_MAX_LIMITED
};

#define TOTAL_RDMA_MEMSIZE	(68 * 1024) /* 68KB */
//RDMA_FMT_YUV_420_P1_8, RDMA_FMT_YUV_420_P1_10 not support by hardware, has checked with asic
//rdma hardware support RDMA_FMT_BGRA_16161616/RDMA_FMT_RGBA_16161616 formats are not  support by fourcc
static const struct dpu_format_id primary_fmts[] = {
	{ DRM_FORMAT_ABGR2101010, 1, 32 }, //RDMA_FMT_ABGR_2101010
	{ DRM_FORMAT_ARGB8888, 4, 32 }, //RDMA_FMT_ARGB_8888
	{ DRM_FORMAT_ABGR8888, 5, 32 }, //RDMA_FMT_ABGR_8888
	{ DRM_FORMAT_RGBA8888, 6, 32 }, //RDMA_FMT_RGBA_8888
	{ DRM_FORMAT_BGRA8888, 7, 32 }, //RDMA_FMT_BGRA_8888
	{ DRM_FORMAT_XRGB8888, 8, 32 }, //RDMA_FMT_XRGB_8888
	{ DRM_FORMAT_XBGR8888, 9, 32 }, //RDMA_FMT_XBGR_8888
	{ DRM_FORMAT_RGBX8888, 10, 32 }, //RDMA_FMT_RGBX_8888
	{ DRM_FORMAT_BGRX8888, 11, 32 }, //RDMA_FMT_BGRX_8888
	{ DRM_FORMAT_RGB565, 22, 16 }, //RDMA_FMT_RGB_565
	{ DRM_FORMAT_BGR565, 23, 16 }, //RDMA_FMT_BGR_565
	{ DRM_FORMAT_RGB888, 12, 24 }, //RDMA_FMT_RGB_888
	{ DRM_FORMAT_BGR888, 13, 24 }, //RDMA_FMT_BGR_888
	/*
	{ DRM_FORMAT_ARGB2101010,	 0 }, //RDMA_FMT_ARGB_2101010
	{ DRM_FORMAT_RGBA1010102,	 2 }, //RDMA_FMT_RGBA_2101010
	{ DRM_FORMAT_BGRA1010102,	 3 }, //RDMA_FMT_BGRA_2101010
	{ DRM_FORMAT_RGB888,		12 }, //RDMA_FMT_RGB_888
	{ DRM_FORMAT_BGR888,        13 }, //RDMA_FMT_BGR_888
	{ DRM_FORMAT_RGBA5551,      14 }, //RDMA_FMT_RGBA_5551
	{ DRM_FORMAT_BGRA5551,      15 }, //RDMA_FMT_BGRA_5551
	{ DRM_FORMAT_ABGR1555,      16 }, //RDMA_FMT_ABGR_1555
	{ DRM_FORMAT_ARGB1555,      17 }, //RDMA_FMT_ARGB_1555
	{ DRM_FORMAT_RGBX5551,      18 }, //RDMA_FMT_RGBX_5551
	{ DRM_FORMAT_BGRX5551,      19 }, //RDMA_FMT_BGRX_5551
	{ DRM_FORMAT_XBGR1555,      20 }, //RDMA_FMT_XBGR_1555
	{ DRM_FORMAT_XRGB1555,      21 }, //RDMA_FMT_XRGB_1555
	{ DRM_FORMAT_ARGB16161616F, 24 }, //RDMA_FMT_ARGB_16161616
	{ DRM_FORMAT_ABGR16161616F, 25 }, //RDMA_FMT_ABGR_16161616
	{ DRM_FORMAT_XYUV8888,      32 }, //RDMA_FMT_XYUV_444_P1_8, uv_swap has no corresponding fourcc format
	{ DRM_FORMAT_Y410,          33 }, //RDMA_FMT_XYUV_444_P1_10, uv_swap has no corresponding fourcc format
	{ DRM_FORMAT_YUYV,          34 }, //RDMA_FMT_VYUY_422_P1_8
	{ DRM_FORMAT_YVYU,          34 }, //RDMA_FMT_VYUY_422_P1_8, uv_swap = 1
	{ DRM_FORMAT_UYVY,          35 }, //RDMA_FMT_YVYU_422_P1_8
	{ DRM_FORMAT_VYUY,          35 }, //RDMA_FMT_YVYU_422_P1_8, uv_swap = 1
	*/
	{ DRM_FORMAT_YUV420_8BIT,   37, 12 }, //DRM_FORMAT_YUV420_8BIT for AFBC
	{ DRM_FORMAT_NV12,          37, 12 }, //RDMA_FMT_YUV_420_P2_8
	{ DRM_FORMAT_YVU420,        38, 12 }, //4CC: YV12

	/*
	{ DRM_FORMAT_NV21,          37 }, //RDMA_FMT_YUV_420_P2_8, uv_swap = 1
	{ DRM_FORMAT_YUV420,        38 }, //RDMA_FMT_YUV_420_P3_8
	{ DRM_FORMAT_YVU420,        38 }, //RDMA_FMT_YUV_420_P3_8, uv_swap = 1
	{ DRM_FORMAT_YUV420_10BIT,  39 }, //RDMA_FMT_YUV_420_P1_10 //DPU not support, DO NOT use
	*/
	{ DRM_FORMAT_P010,          40, 24 }, //RDMA_FMT_YUV_420_P2_10
	/*
	{ DRM_FORMAT_Q410,          41 }, //DPU not support
	{ DRM_FORMAT_Q401,          41 }, //DPU not support
	*/
};

/* hee */
const struct spacemit_hw_rdma saturn_hee_rdmas[] = {
	/* TODO: set max_yuv_height to 1088 instead of 1080 due to vpu fw issue */
	{FORMAT_RGB | FORMAT_AFBC, ROTATE_COMMON},
	{FORMAT_RGB | FORMAT_AFBC | FORMAT_RAW_YUV | FORMAT_AFBC, ROTATE_COMMON | ROTATE_AFBC_90_270},
	{FORMAT_RGB | FORMAT_AFBC, ROTATE_COMMON},
	{FORMAT_RGB | FORMAT_AFBC, ROTATE_COMMON},
};

const u32 saturn_hee_fbcmem_sizes[] = {
	89600,	//87.5k
	15360,	//16k
};

const struct spacemit_hw_device spacemit_dp_devices[SPACEMIT_DP_MAX_DEVICES] = {
	[SATURN_HEE] = {
		.plane_nums = 16,
		.offline_plane_nums = 1,
		.crtc_nums = 2,
		.rdma_nums = ARRAY_SIZE(saturn_hee_rdmas),
		.rdmas = saturn_hee_rdmas,
		.n_formats = ARRAY_SIZE(primary_fmts),
		.formats = primary_fmts,
		.n_fbcmems = ARRAY_SIZE(saturn_hee_fbcmem_sizes),
		.fbcmem_sizes = saturn_hee_fbcmem_sizes,
		.solid_color_shift = 0,
		.scaler_num = 1,
		.update_csc_matrix = saturn_hee_update_csc_matrix,
		.conf_scaler_x = saturn_hee_conf_scaler_x,
		.enable_vsync = saturn_hee_enable_vsync,
		.enable_cfg_irq = saturn_hee_enable_cfg_irq,
		.cfg_ready = saturn_hee_cfg_ready,
		.sw_start = saturn_hee_sw_start,
		.irq_enable = saturn_hee_irq_enable,
		.dpu_init = saturn_hee_dpu_init,
		.plane_update_hw_channel = saturn_hee_plane_update_hw_channel,
		.plane_disable_hw_channel = saturn_hee_plane_disable_hw_channel,
		.conf_dpuctrl = saturn_hee_conf_dpuctrl,
		.get_cfg_rdy = saturn_hee_get_cfg_rdy,
		.get_int_sts = saturn_hee_get_int_sts,
		.get_irq_bit = saturn_hee_get_irq_bit,
		.clr_int_sts = saturn_hee_clr_int_sts,
		.dpu_disable = saturn_hee_dpu_disable,
		.dpu_restart = saturn_hee_dpu_restart,
		.dpu_stop_check = saturn_hee_dpu_stop_check,
		.enable_cmdlist = saturn_hee_enable_cmdlist,
		.cfg_cmdlist = saturn_hee_cfg_cmdlist,
		.rdma_dmmu = saturn_hee_rdma_dmmu,
		.get_cl_rdma_buf = saturn_hee_get_cl_rdma_buf,
		.cmdlist_fill_data_row = saturn_hee_cmdlist_fill_data_row,
		.cmdlist_fill_conf_row = saturn_hee_cmdlist_fill_conf_row,
		.crtc_cmdlist = saturn_hee_crtc_cmdlist,
		.cmdlist_dump_node = saturn_hee_cmdlist_dump_node,
	},
	[SATURN_EDP] = {
		.plane_nums = 16,
		.offline_plane_nums = 1,
		.crtc_nums = 2,
		.rdma_nums = ARRAY_SIZE(saturn_hee_rdmas),
		.rdmas = saturn_hee_rdmas,
		.n_formats = ARRAY_SIZE(primary_fmts),
		.formats = primary_fmts,
		.n_fbcmems = ARRAY_SIZE(saturn_hee_fbcmem_sizes),
		.fbcmem_sizes = saturn_hee_fbcmem_sizes,
		.solid_color_shift = 0,
		.scaler_num = 1,
		.update_csc_matrix = saturn_hee_update_csc_matrix,
		.conf_scaler_x = saturn_hee_conf_scaler_x,
		.enable_vsync = saturn_hee_enable_vsync,
		.enable_cfg_irq = saturn_hee_enable_cfg_irq,
		.cfg_ready = saturn_hee_cfg_ready,
		.sw_start = saturn_hee_sw_start,
		.irq_enable = saturn_hee_irq_enable,
		.dpu_init = saturn_hee_dpu_init,
		.plane_update_hw_channel = saturn_hee_plane_update_hw_channel,
		.plane_disable_hw_channel = saturn_hee_plane_disable_hw_channel,
		.conf_dpuctrl = saturn_hee_conf_dpuctrl,
		.get_cfg_rdy = saturn_hee_get_cfg_rdy,
		.get_int_sts = saturn_hee_get_int_sts,
		.get_irq_bit = saturn_hee_get_irq_bit,
		.clr_int_sts = saturn_hee_clr_int_sts,
		.dpu_disable = saturn_hee_dpu_disable,
		.dpu_restart = saturn_hee_dpu_restart,
		.dpu_stop_check = saturn_hee_dpu_stop_check,
		.enable_cmdlist = saturn_hee_enable_cmdlist,
		.cfg_cmdlist = saturn_hee_cfg_cmdlist,
		.rdma_dmmu = saturn_hee_rdma_dmmu,
		.get_cl_rdma_buf = saturn_hee_get_cl_rdma_buf,
		.cmdlist_fill_data_row = saturn_hee_cmdlist_fill_data_row,
		.cmdlist_fill_conf_row = saturn_hee_cmdlist_fill_conf_row,
		.crtc_cmdlist = saturn_hee_crtc_cmdlist,
		.cmdlist_dump_node = saturn_hee_cmdlist_dump_node,
	},
};

void saturn_enable_irq_mask(struct spacemit_crtc *a_crtc, bool enable, u32 offset, u32 mask)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	//unsigned long flags;

	if (!hwdev->base) {
		DRM_ERROR("hwdev->base NULL %s\n", __func__);
		return;
	}

	if (!a_crtc->power_on)
		return;

	regmap_update_bits(hwdev->regmap, offset, mask, enable ? mask : 0);
}

void spacemit_dpu_power_enable(struct spacemit_crtc *a_crtc, bool enable)
{
	if (enable) {
		if (a_crtc->power_on)
			return;

		pm_runtime_get_sync(a_crtc->dev);
		a_crtc->power_on = true;
	} else {
		if (!a_crtc->power_on)
			return;

		a_crtc->power_on = false;
		pm_runtime_put_sync(a_crtc->dev);

	}

}

static int dpu_parse_dt(struct spacemit_crtc *a_crtc, struct device_node *np)
{
	struct dpu_clk_context *clk_ctx = &a_crtc->clk_ctx;

	clk_ctx->pxclk = devm_clk_get(a_crtc->dev, "pxclk");
	if (IS_ERR(clk_ctx->pxclk))
		return dev_err_probe(a_crtc->dev, PTR_ERR(clk_ctx->pxclk),
				     "Failed to get pxclk\n");

	clk_ctx->mclk = devm_clk_get(a_crtc->dev, "mclk");
	if (IS_ERR(clk_ctx->mclk))
		return dev_err_probe(a_crtc->dev, PTR_ERR(clk_ctx->mclk),
				     "Failed to get mclk\n");

	clk_ctx->escclk = devm_clk_get_optional(a_crtc->dev, "escclk");
	if (IS_ERR(clk_ctx->escclk))
		return dev_err_probe(a_crtc->dev, PTR_ERR(clk_ctx->escclk),
				     "Failed to get escclk\n");

	clk_ctx->aclk = devm_clk_get_optional(a_crtc->dev, "aclk");
	if (IS_ERR(clk_ctx->aclk))
		return dev_err_probe(a_crtc->dev, PTR_ERR(clk_ctx->aclk),
				     "Failed to get aclk\n");

	clk_ctx->dscclk = devm_clk_get_optional(a_crtc->dev, "dscclk");
	if (IS_ERR(clk_ctx->dscclk))
		return dev_err_probe(a_crtc->dev, PTR_ERR(clk_ctx->dscclk),
				     "Failed to get dscclk\n");

	a_crtc->aclk = DPU_AXICLK_DEFAULT;

	return 0;
}

static int dpu_enable_clocks(struct spacemit_crtc *a_crtc)
{
	struct dpu_clk_context *clk_ctx = &a_crtc->clk_ctx;
	struct drm_crtc *crtc = &a_crtc->crtc;
	struct drm_display_mode *mode = &crtc->mode;
	uint64_t clk_val;
	uint64_t set_clk_val;

	clk_prepare_enable(clk_ctx->pxclk);
	set_clk_val = mode->clock * 1000;
	if (set_clk_val) {
		set_clk_val = clk_round_rate(clk_ctx->pxclk, set_clk_val);
		clk_val = clk_get_rate(clk_ctx->pxclk);
		if (clk_val != set_clk_val) {
			clk_set_rate(clk_ctx->pxclk, set_clk_val);
			pr_debug("pxclk=%lld\n", clk_val);
		}
	}

	clk_prepare_enable(clk_ctx->mclk);
	clk_val = clk_get_rate(clk_ctx->mclk);
	if (clk_val != DPU_MCLK_DEFAULT) {
		clk_val = clk_round_rate(clk_ctx->mclk, DPU_MCLK_DEFAULT);
		clk_set_rate(clk_ctx->mclk, clk_val);
		pr_debug("mclk=%lld\n", clk_val);
	}

	if (clk_ctx->escclk) {
		clk_prepare_enable(clk_ctx->escclk);

		clk_val = clk_get_rate(clk_ctx->escclk);
		set_clk_val = a_crtc->escclk;
		if (clk_val != set_clk_val) {
			clk_val = clk_round_rate(clk_ctx->escclk, set_clk_val);
			clk_set_rate(clk_ctx->escclk, clk_val);
			pr_debug("escclk=%lld\n", clk_val);
		}
	}

	if (clk_ctx->aclk) {
		clk_prepare_enable(clk_ctx->aclk);

		clk_val = clk_get_rate(clk_ctx->aclk);
		set_clk_val = a_crtc->aclk;
		pr_info("Current aclk rate: %llu, Target: %llu\n", clk_val, set_clk_val);

		if (clk_val != set_clk_val) {
			uint64_t orig_rate = clk_val;
			uint64_t rounded = clk_round_rate(clk_ctx->aclk, set_clk_val);
			pr_info("aclk rounded rate: %llu (original current: %llu)\n", rounded, orig_rate);
			if (rounded != orig_rate) {
				pr_info("aclk clk_set_rate: %llu -> %llu\n", orig_rate, rounded);
				clk_set_rate(clk_ctx->aclk, rounded);
				pr_info("aclk after clk_set_rate: %llu\n", (u64)clk_get_rate(clk_ctx->aclk));
			} else {
				pr_info("aclk set_rate skipped: rounded rate %llu already active\n", rounded);
			}
		}
	}

	if (clk_ctx->dscclk)
		clk_prepare_enable(clk_ctx->dscclk);

	trace_dpu_enable_clocks(a_crtc->dev_id);
	pr_info("%s() done\n", __func__);
	return 0;
}

static int dpu_disable_clocks(struct spacemit_crtc *a_crtc)
{
	struct dpu_clk_context *clk_ctx = &a_crtc->clk_ctx;

	trace_dpu_disable_clocks(a_crtc->dev_id);

	clk_disable_unprepare(clk_ctx->pxclk);
	clk_disable_unprepare(clk_ctx->mclk);
	clk_disable_unprepare(clk_ctx->escclk);

	clk_disable_unprepare(clk_ctx->aclk);
	clk_disable_unprepare(clk_ctx->dscclk);

	return 0;
}

u8 spacemit_plane_hw_get_format_id(u32 format)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(primary_fmts); i++) {
		if (primary_fmts[i].format == format)
			return primary_fmts[i].id;
	}

	return SPACEMIT_DPU_INVALID_FORMAT_ID;
}

void spacemit_get_afbc_modifier(uint64_t modifier, struct spacemit_afbc_state *afbc_state)
{
	uint64_t super_block_size = modifier & AFBC_FORMAT_MOD_BLOCK_SIZE_MASK;

	if (super_block_size == AFBC_FORMAT_MOD_BLOCK_SIZE_16x16)
		afbc_state->block_size = 0;
	else if (super_block_size == AFBC_FORMAT_MOD_BLOCK_SIZE_32x8)
		afbc_state->block_size = 1;

	if (modifier & AFBC_FORMAT_MOD_TILED)
		afbc_state->tile_type = 1;
	else
		afbc_state->tile_type = 0;

	if (modifier & AFBC_FORMAT_MOD_YTR)
		afbc_state->yuv_transform = 1;
	else
		afbc_state->yuv_transform = 0;

	if (modifier & AFBC_FORMAT_MOD_SPLIT)
		afbc_state->split_mode = 1;
	else
		afbc_state->split_mode = 0;

	if (modifier & AFBC_FORMAT_MOD_CBR)
		afbc_state->copy_mode = 1;
	else
		afbc_state->copy_mode = 0;

}

static void saturn_enable_vsync(struct spacemit_crtc *a_crtc, bool enable)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	trace_saturn_enable_vsync("vsync", enable);
	hwdev->enable_vsync(a_crtc, hwdev, enable);
}

void spacemit_cfg_rdy_timer_handler(struct timer_list *t)
{
	struct spacemit_crtc *a_crtc = timer_container_of(a_crtc, t, cfg_rdy_timer);

	DRM_ERROR("CFG_RDY ERROR:%s\n", __func__);
	trace_saturn_ctrl_cfg_ready_timer(a_crtc->dev_id);
	a_crtc->flip_done = false;
}

static void saturn_ctrl_cfg_ready(struct spacemit_crtc *a_crtc, bool enable)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	if (a_crtc->is_stopped)
		return;

	trace_saturn_ctrl_cfg_ready(a_crtc->dev_id, enable);
	mod_timer(&a_crtc->cfg_rdy_timer, jiffies + msecs_to_jiffies(3000));
	hwdev->cfg_ready(a_crtc, hwdev);

}

static void saturn_ctrl_sw_start(struct spacemit_crtc *a_crtc, bool enable)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	trace_saturn_ctrl_sw_start(a_crtc->dev_id, enable);

	hwdev->sw_start(a_crtc, hwdev);
}

static u32 dpu_get_version(struct spacemit_crtc *a_crtc)
{
	return 0;
}

static int dpu_init(struct spacemit_crtc *a_crtc)
{
	unsigned int timeout = 1000;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	if (!a_crtc->power_on)
		return 0;

	while (timeout) {
		if (hwdev->get_cfg_rdy(a_crtc, hwdev) == 0)
			break;
		udelay(100);
		timeout--;
	};
	if (timeout == 0)
		DRM_ERROR("%s wait cfg ready done timeout\n", __func__);

	hwdev->dpu_init(a_crtc);

	return 0;
}

static void dpu_uninit(struct spacemit_crtc *a_crtc)
{
        struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
        struct spacemit_hw_device *hwdev = priv->hwdev;

        if (!a_crtc->power_on)
                return;

        trace_dpu_uninit(a_crtc->dev_id);
        hwdev->irq_enable(a_crtc, false);
}
static inline void dpu_isr_vblank(struct spacemit_crtc *a_crtc, bool *flip)
{
	struct drm_crtc *crtc = &a_crtc->crtc;

	drm_crtc_handle_vblank(crtc);
	if (!*flip) {
		struct drm_device *drm = a_crtc->crtc.dev;
		struct drm_pending_vblank_event *event = crtc->state->event;

		*flip = true;
		spin_lock(&drm->event_lock);
		if (crtc->state->event) {
			/*
			 * Set event to NULL first to ensure event is consumed
			 * before drm_atomic_helper_commit_hw_done.
			 */
			crtc->state->event = NULL;
			drm_crtc_send_vblank_event(crtc, event);
		}
		spin_unlock(&drm->event_lock);
		drm_crtc_vblank_put(crtc);
	}
}

u32 saturn_conf_dpuctrl_rdma(struct spacemit_crtc *a_crtc)
{
	struct drm_crtc *crtc = &a_crtc->crtc;
	struct drm_plane *plane;
	u32 rdma_en = 0;

	/* Find out the active rdmas */
	drm_atomic_crtc_for_each_plane(plane, crtc) {
		u32 rdma_id = to_spacemit_plane_state(plane->state)->rdma_id;

		if (rdma_id != RDMA_INVALID_ID)
			rdma_en |= (1 << rdma_id);
	}

	trace_dpuctrl("rdma_en", rdma_en);

	return rdma_en;
}

static uint32_t dpu_online_isr(struct spacemit_crtc *a_crtc)
{
	uint32_t irq_raw, irq_bit, irq_ur_bit, irq_dbg_sts;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	static bool ur_done;
	int dev_id = a_crtc->dev_id;

	trace_dpu_isr(a_crtc->dev_id);

	irq_raw = hwdev->get_int_sts(hwdev, dev_id);
	trace_dpu_isr_status("ONLINE", irq_raw);
	/* underrun */
	irq_bit = hwdev->get_irq_bit(INT_UNDERRUN, dev_id);
	irq_ur_bit = irq_raw & irq_bit;
	if (irq_ur_bit && !ur_done) {
		hwdev->clr_int_sts(a_crtc, irq_ur_bit, dev_id);
		trace_dpu_isr_status("Under Run!", irq_ur_bit);
		DRM_ERROR_RATELIMITED("Under Run!\n");
#if IS_ENABLED(CONFIG_SPACEMIT_CORE_CLK_DOVE) || IS_ENABLED(CONFIG_SPACEMIT_CORE_CLK_LARK)
		DRM_ERROR_RATELIMITED("DDR Freq = %d\n", ddr_get_freq_lv());
#endif
		ur_done = true;
	}
	/* cfg ready clear */
	irq_bit = hwdev->get_irq_bit(INT_CFG_RDY, dev_id);
	if (irq_raw & irq_bit) {
		hwdev->clr_int_sts(a_crtc, irq_bit, dev_id);
		trace_dpu_isr_status("cfg_rdy_clr", irq_raw & irq_bit);
		a_crtc->flip_done = false;
		timer_delete(&a_crtc->cfg_rdy_timer);
		ur_done = false;
		if (hwdev->enable_cfg_irq)
			hwdev->enable_cfg_irq(a_crtc, hwdev, false);
	}
	/* vsync */
	irq_bit = hwdev->get_irq_bit(INT_VSYNC, dev_id);
	if (irq_raw & irq_bit) {
		hwdev->clr_int_sts(a_crtc, irq_bit, dev_id);
		trace_dpu_isr_status("vsync", irq_raw & irq_bit);
		dpu_isr_vblank(a_crtc, &a_crtc->flip_done);
	}
	/* rest irq status */
	irq_bit = hwdev->get_irq_bit(INT_REST, dev_id) | irq_ur_bit;
	if (irq_raw & irq_bit)
		hwdev->clr_int_sts(a_crtc, irq_bit, dev_id);

	/* vsync update clear */
	irq_bit = hwdev->get_irq_bit(INT_VSYNC_UPDATE, dev_id);
	if (irq_raw & irq_bit)
		hwdev->clr_int_sts(a_crtc, irq_bit, dev_id);

	if (hwdev->get_rdma_dbg_sts) {
		irq_dbg_sts = hwdev->get_rdma_dbg_sts(a_crtc, dev_id);
		if (irq_dbg_sts)
			trace_dpu_isr_status("ONLINE INT DBG STS", irq_dbg_sts);
	}

	return 0;
}

static void dpu_run(struct drm_crtc *crtc,
		    struct drm_crtc_state *old_state)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	trace_dpu_run(a_crtc->dev_id);

	/* config dpuctrl modules */
	if (hwdev->conf_dpuctrl)
		hwdev->conf_dpuctrl(crtc, old_state);

	//dsb(sy);
	mb();

	drm_crtc_vblank_get(crtc);
	if (hwdev->enable_cfg_irq)
		hwdev->enable_cfg_irq(a_crtc, hwdev, true);

	saturn_ctrl_cfg_ready(a_crtc, true);

	if (unlikely(a_crtc->is_1st_f)) {
		pr_info("DPU %d Start!\n", a_crtc->dev_id);
		a_crtc->is_1st_f = false;
		saturn_ctrl_sw_start(a_crtc, true);
	}
#ifdef CONFIG_ARM64
	__iomb();
#else
	dma_rmb();
#endif
}

static void dpu_esd_restart(struct spacemit_crtc *a_crtc)
{
	unsigned int timeout = DPU_STOP_TIMEOUT;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	struct drm_crtc *crtc = &a_crtc->crtc;

	pr_info("%s()\n", __func__);

	while (timeout) {
		if (hwdev->get_cfg_rdy(a_crtc, hwdev) == 0)
			break;
		udelay(10);
		timeout--;
	};
	if (timeout == 0)
		DRM_ERROR("%s dpu %d wait cfg ready done timeout\n", __func__, a_crtc->dev_id);
	else
		pr_debug("%s dpu %d wait cfg ready done %d\n", __func__, timeout, a_crtc->dev_id);

	hwdev->dpu_restart(a_crtc, hwdev);
	drm_crtc_vblank_get(crtc);
	if (hwdev->enable_cfg_irq)
		hwdev->enable_cfg_irq(a_crtc, hwdev, true);
	//dsb(sy);
	mb();
	hwdev->cfg_ready(a_crtc, hwdev);
	hwdev->sw_start(a_crtc, hwdev);
#ifdef CONFIG_ARM64
	__iomb();
#else
	dma_rmb();
#endif

	timeout = hwdev->reboot_flag ? DPU_STOP_REBOOT_TIMEOUT : DPU_STOP_TIMEOUT;
	while (timeout) {
		if (hwdev->get_cfg_rdy(a_crtc, hwdev) & 1) {
			udelay(10);
			timeout--;
			continue;
		} else
			break;
	};
	if (timeout == 0)
		DRM_ERROR("%s esd_restart timeout!!!\n", __func__);
	else
		pr_debug("%s esd_restart done %d\n", __func__, timeout);
	hwdev->irq_enable(a_crtc, 1);
}

static void dpu_stop(struct spacemit_crtc *a_crtc)
{
	unsigned int timeout = DPU_STOP_TIMEOUT;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	int ret = 0;

	if (!a_crtc->power_on)
		return;

	pr_info("%s()\n", __func__);

	trace_dpu_stop(a_crtc->dev_id);

	hwdev->irq_enable(a_crtc, 0);
	while (timeout) {
		if (hwdev->get_cfg_rdy(a_crtc, hwdev) == 0)
			break;
		udelay(10);
		timeout--;
	};
	if (timeout == 0) {
		DRM_ERROR("%s dpu %d wait cfg ready done timeout\n", __func__, a_crtc->dev_id);
		a_crtc->stop_to++;
	} else
		pr_debug("%s dpu %d wait cfg ready done %d\n", __func__, timeout, a_crtc->dev_id);

	hwdev->dpu_disable(a_crtc, hwdev);
	//dsb(sy);
	mb();
	hwdev->cfg_ready(a_crtc, hwdev);
#ifdef CONFIG_ARM64
	__iomb();
#else
	dma_rmb();
#endif

	timeout = hwdev->reboot_flag ? DPU_STOP_REBOOT_TIMEOUT : DPU_STOP_TIMEOUT;
	while (timeout) {
		if (hwdev->get_cfg_rdy(a_crtc, hwdev) & 1) {
			udelay(10);
			timeout--;
			continue;
		} else
			break;
	};
	ret = hwdev->dpu_stop_check(a_crtc, hwdev);
	if (ret) {
		DRM_ERROR("dpu %d stop check failed!!!\n", a_crtc->dev_id);
		a_crtc->stop_to++;
	}
	if (timeout == 0) {
		DRM_ERROR("%s dpu %d stop timeout!!!\n", __func__, a_crtc->dev_id);
		a_crtc->stop_to++;
		pm_stay_awake(a_crtc->dev);
	} else {
		pr_debug("%s dpu %d stop Done %d\n", __func__, a_crtc->dev_id, timeout);
		if (a_crtc->stop_to) {
			pm_relax(a_crtc->dev);
			a_crtc->stop_to = 0;
		}
	}

}

static int dpu_modeset(struct spacemit_crtc *a_crtc, struct drm_mode_modeinfo *mode)
{
	return 0;
}

static void dpu_enable_vsync(struct spacemit_crtc *a_crtc)
{
	saturn_enable_vsync(a_crtc, true);
}

static void dpu_disable_vsync(struct spacemit_crtc *a_crtc)
{
	saturn_enable_vsync(a_crtc, false);
}

const struct dpu_core_ops dpu_saturn_ops = {
	.parse_dt = dpu_parse_dt,
	.version = dpu_get_version,
	.init = dpu_init,
	.uninit = dpu_uninit,
	.run = dpu_run,
	.stop = dpu_stop,
	.esd_restart = dpu_esd_restart,
	.online_isr = dpu_online_isr,
	.modeset = dpu_modeset,
	.enable_clk = dpu_enable_clocks,
	.disable_clk = dpu_disable_clocks,
	.enable_vsync = dpu_enable_vsync,
	.disable_vsync = dpu_disable_vsync,
	.cal_layer_fbcmem_size = saturn_cal_layer_fbcmem_size,
	.adjust_rdma_fbcmem = saturn_adjust_rdma_fbcmem,
};

MODULE_LICENSE("GPL v2");
