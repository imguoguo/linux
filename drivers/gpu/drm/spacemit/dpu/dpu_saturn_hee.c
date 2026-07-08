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
#include <linux/pm_qos.h>
#include <linux/regmap.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_blend.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem.h>
#include "dpu_saturn.h"
#include "saturn_fbcmem.h"
#include "../spacemit_cmdlist.h"
#include "../spacemit_dmmu.h"
#include "../spacemit_dpu_reg.h"
#include <video/display_timing.h>
#include <dt-bindings/display/spacemit_dpu.h>
#include "saturn_regs/ops_hee.h"
#include "saturn_regs/reg_map_hee.h"

#include "dpu_trace.h"

const u32 PREPIPE_SCAL_BASE_ADDR[] = {
	PREPIPE_SCAL0_BASE_ADDR,
	PREPIPE_SCAL1_BASE_ADDR,
	PREPIPE_SCAL2_BASE_ADDR,
	PREPIPE_SCAL3_BASE_ADDR,
};

enum RDMA_ID {
	DPU_RDMA0,
	DPU_RDMA1,
	DPU_RDMA2,
	DPU_RDMA3,
	DPU_RDMA_MAX,
	DPU_RDMA_INVALID,
};

/* hee:
 * dpu_write:  uniform api for both cpu and cmdlist config dpu registers
 */

#define SPACEMIT_YUV2RGB_COEFFS 12
#define ROT_MODE_MIRROR	4
#define ROT_MODE_FLIP	5
#define ROT_MODE_MIRROR_90	6
#define ROT_MODE_FLIP_90	7
#define hee_SCL_SEL_ID_INVALID  9
#define POST_SCALER_POS	6
#define POST_SCL 2

static bool ur_enabhee = true;

static const s16
spacemit_yuv2rgb_coefs[DRM_COLOR_ENCODING_MAX][DRM_COLOR_RANGE_MAX][SPACEMIT_YUV2RGB_COEFFS] = {
	[DRM_COLOR_YCBCR_BT601][DRM_COLOR_YCBCR_LIMITED_RANGE] = {
		1192,    0, 1635, -223,
		1192, -403, -833, 136,
		1192, 2065,    0, -277,
	},
	[DRM_COLOR_YCBCR_BT601][DRM_COLOR_YCBCR_FULL_RANGE] = {
		1024,    0, 1436, -179,
		1024, -354, -732,  136,
		1024, 1814,    0, -227,
	},
	[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE] = {
		1192,    0, 1836, -248,
		1192, -218, -546,   77,
		1192, 2163,    0, -289,
	},
	[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_FULL_RANGE] = {
		1024,	 0, 1613, -202,
		1024, -192, -479,   84,
		1024, 1900,    0, -238,
	},
	[DRM_COLOR_YCBCR_BT2020][DRM_COLOR_YCBCR_LIMITED_RANGE] = {
		1196,	 0, 1724,  -937,
		1196, -192, -668,   355,
		1196, 2200,    0, -1175,
	},
	[DRM_COLOR_YCBCR_BT2020][DRM_COLOR_YCBCR_FULL_RANGE] = {
		1024,    0, 1510, -755,
		1024, -168, -585,  377,
		1024, 1927,    0, -963,
	}
};

#define CONFIG_TBU_REGS(hwdev, base, reg_id, cl_tbu) \
{\
	dpu_write(hwdev, MMU_TBU_REG, base, tbu_base_addr##reg_id##_low, \
		  tbu->ttb_pa[reg_id] & 0xFFFFFFFF, cl_tbu, 1 + 2 * reg_id); \
	dpu_write(hwdev, MMU_TBU_REG, base, tbu_base_addr##reg_id##_high, \
		  (tbu->ttb_pa[reg_id] >> 32) & 0xFFF, cl_tbu, 2 + 2 * reg_id); \
	dpu_write(hwdev, MMU_TBU_REG, base, tbu_va##reg_id, \
		 (tbu->tbu_va[reg_id] >> 12), cl_tbu, 7 + reg_id); \
	dpu_write(hwdev, MMU_TBU_REG, base, \
		  tbu_size##reg_id, tbu->ttb_size[reg_id], cl_tbu, 10 + reg_id); \
	dpu_write(hwdev, MMU_TBU_REG, base, \
		  vsync_update_en, 1, cl_tbu, 13); \
}

#define CONFIG_RDMA_ADDR_REG(hwdev, reg_id, rdma_id, addr, cl_rdma) \
{ \
	if (hwdev) { \
		dpu_write(hwdev, RDMA_PATH_X_REG, RDMA_BASE_ADDR[rdma_id], \
			  base_addr##reg_id##_low_ly0, addr & 0xFFFFFFFF, \
			  cl_rdma, 9 + 2 * reg_id); \
		dpu_write(hwdev, RDMA_PATH_X_REG, RDMA_BASE_ADDR[rdma_id], \
			  base_addr##reg_id##_high_ly0, (addr >> 32) & 0xFFF, \
			  cl_rdma, 10 + 2 * reg_id); \
	} \
}

static const u32 RDMA_BASE_ADDR[] = {
	RDMA0_BASE_ADDR,
	RDMA1_BASE_ADDR,
	RDMA2_BASE_ADDR,
	RDMA3_BASE_ADDR,
};


static const u32 CMP_BASE_ADDR[] = {
	CMP0_BASE_ADDR,
	CMP1_BASE_ADDR,
	CMP2_BASE_ADDR,
};

static const u32 TMG_BASE_ADDR[] = {
	0,
	TMG0_BASE_ADDR,
	0,
};

static const u32 MMU_TBU_BASE_ADDR_ARRAY[] = {
	TBU0_ADDR, TBU1_ADDR, TBU2_ADDR, TBU3_ADDR,
	TBU4_ADDR, TBU5_ADDR, TBU6_ADDR, TBU7_ADDR,
};

void saturn_hee_update_csc_matrix(struct drm_plane *plane, struct drm_plane_state *old_state)
{
	struct spacemit_plane_state *spacemit_plane_state = to_spacemit_plane_state(plane->state);
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u32 rdma_id = spacemit_plane_state->rdma_id;
	u32 module_base;
	int color_encoding = plane->state->color_encoding;
	int color_range = plane->state->color_range;
	int value;
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(plane->state->crtc);
	struct cmdlist_regs *cl_rdma = a_crtc->cl_rdma;

	if (rdma_id == RDMA_INVALID_ID)
		return;

	module_base = RDMA_BASE_ADDR[rdma_id];

	if ((color_encoding != old_state->color_encoding) || (color_range != old_state->color_range)) {
		int i, index = 33;

		for (i = 0; i < 12; i += 2, index++) {
			value = (spacemit_yuv2rgb_coefs[color_encoding][color_range][i] & 0x3FFF) |
				((spacemit_yuv2rgb_coefs[color_encoding][color_range][i + 1] & 0x3FFF) << 16);
			dpu_write(hwdev, RDMA_PATH_X_REG, module_base, value32[index], value, cl_rdma, index);
		}
	}
}

void saturn_hee_conf_scaler_x(struct drm_plane_state *state, struct cmdlist_regs *cl)
{
	struct spacemit_plane_state *spacemit_plane_state = to_spacemit_plane_state(state);
	struct drm_plane *plane = state->plane;
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u32 in_width, in_height, out_width, out_height;
	uint32_t hor_delta_phase, ver_delta_phase;
	int64_t hor_init_phase, ver_init_phase;
	uint32_t hor_init_phase_h1b, ver_init_phase_h1b;
	bool hor_enable = false, ver_enable = false;
	u32 module_base;
	struct cmdlist_regs *cl_scl = NULL;
	cl_scl = alloc_cmdlist_regs(SCALER_X_REG);

	trace_saturn_conf_scaler_x(spacemit_plane_state);

	if (is_rot_90_270(state->rotation)) {
		in_width  = state->src_h >> 16;
		in_height = state->src_w >> 16;
	} else {
		in_width  = state->src_w >> 16;
		in_height = state->src_h >> 16;
	}

	out_width = state->crtc_w;
	out_height = state->crtc_h;

	hor_enable = out_width == in_width ? false : true;
	ver_enable = out_height == in_height ? false : true;
	module_base = PREPIPE_SCAL_BASE_ADDR[spacemit_plane_state->rdma_id];

	hor_delta_phase = in_width * 65536 / out_width;
	ver_delta_phase = in_height * 65536 / out_height;

	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_enable, hor_enable, cl_scl, 1);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_enable, ver_enable, cl_scl, 1);

	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_delta_phase, hor_delta_phase, cl_scl, 7);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_delta_phase, ver_delta_phase, cl_scl, 8);

	hor_init_phase = ((int64_t)hor_delta_phase - 65536) >> 1;
	ver_init_phase = ((int64_t)ver_delta_phase - 65536) >> 1;

	DRM_DEBUG("hor_delta:%d ver_delta:%d\n", hor_delta_phase, ver_delta_phase);
	DRM_DEBUG("hor_init_phase:%lld ver_init_phase:%lld\n", hor_init_phase, ver_init_phase);

	if (hor_init_phase >= 0) {
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_init_phase_l32b, hor_init_phase & 0x00000000ffffffff, cl_scl, 3);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_init_phase_h1b, 0, cl_scl, 4);
	} else { //convert to positive value if negative
		hor_init_phase += 8589934592;
		hor_init_phase_h1b = (uint32_t)((hor_init_phase & 0x0000000100000000ULL) >> 32);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_init_phase_l32b, hor_init_phase & 0x00000000ffffffff, cl_scl, 3);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_init_phase_h1b, hor_init_phase_h1b, cl_scl, 4);
	}

	if (ver_init_phase >= 0) {
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_init_phase_l32b, ver_init_phase & 0x00000000ffffffff, cl_scl, 5);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_init_phase_h1b, 0, cl_scl, 6);
	} else { //convert to positive value if negative
		ver_init_phase += 8589934592;
		ver_init_phase_h1b = (uint32_t)((ver_init_phase & 0x0000000100000000ULL) >> 32);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_init_phase_l32b, ver_init_phase & 0x00000000ffffffff, cl_scl, 5);
		dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_init_phase_h1b, ver_init_phase_h1b, cl_scl, 6);
	}

	// enable both ver and hor
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_hor_enable, 0x1, cl_scl, 0);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_ver_enable, 0x1, cl_scl, 0);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_input_width, in_width, cl_scl, 1);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_input_height, in_height, cl_scl, 1);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_output_width, out_width, cl_scl, 2);
	dpu_write(hwdev, SCALER_X_REG, module_base, b.m_nscl_output_height, out_height, cl_scl, 2);

	cmdlist_regs_packing(plane_to_cl(plane), CMDLIST_MOD_SCL, cl_scl);
	free_cmdlist_regs(cl_scl);
}

void saturn_hee_enable_vsync(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, bool enable)
{
	u32 offset = DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_MSK;
	u32 mask = saturn_hee_get_irq_bit(INT_VSYNC, a_crtc->dev_id);

	saturn_enable_irq_mask(a_crtc, enable, offset, mask);
}

void saturn_hee_enable_cfg_irq(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, bool enable)
{
	u32 offset = DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_MSK;
	u32 mask = saturn_hee_get_irq_bit(INT_CFG_RDY, a_crtc->dev_id);

	saturn_enable_irq_mask(a_crtc, enable, offset, mask);
}

void saturn_hee_cfg_ready(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	u32 base = DPU_SCENE_CTL_ADDR(a_crtc->dev_id);

	saturn_enable_irq_mask(a_crtc, true, DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_MSK, saturn_hee_get_irq_bit(INT_UNDERRUN, a_crtc->dev_id));
	ur_enabhee = true;
	dpu_write(hwdev, DPU_CTL_REG, base, both_cfg_rdy, 1);
}

void saturn_hee_sw_start(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	u32 base;
	base = DPU_SCENE_CTL_ADDR(a_crtc->dev_id);
	dpu_write_reg(hwdev, DPU_CTL_REG, base, sw_start, 1);
}

static void saturn_init_csc(struct spacemit_crtc *a_crtc)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u32 module_base;
	int i = 0;

	hwdev->color_encoding = DRM_COLOR_YCBCR_BT709;
	hwdev->color_range = DRM_COLOR_YCBCR_LIMITED_RANGE;

	for (i = 0; i < hwdev->rdma_nums; i++) {
		module_base = RDMA_BASE_ADDR[i];
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix00, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][0] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix01, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][1] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix02, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][2] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix03, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][3] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix10, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][4] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix11, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][5] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix12, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][6] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix13, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][7] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix20, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][8] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix21, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][9] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix22, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][10] & 0x3FFF);
		dpu_write(hwdev, RDMA_PATH_X_REG, module_base, csc_matrix23, spacemit_yuv2rgb_coefs[DRM_COLOR_YCBCR_BT709][DRM_COLOR_YCBCR_LIMITED_RANGE][11] & 0x3FFF);
	}
}

static void saturn_init_tmg(struct spacemit_crtc *a_crtc)
{
	u32 base = 0;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	struct drm_crtc *crtc = &a_crtc->crtc;
	struct drm_display_mode *mode = &crtc->mode;
	u16 vfp, vbp, vsync, hfp, hbp, hsync;
	void __iomem *tmg_addr;
	u32 value;

	/*
	 * The TMG block sits 0x51200 bytes into each DPU MMIO region
	 * (DPU0 base 0xc0340000 -> tmg 0xc0391200; DPU1 base 0xc0440000
	 * -> tmg 0xc0491200). Derive the per-instance address from
	 * hwdev->phy_addr rather than re-using ::dev_id (which now
	 * names the composer type, not the DPU instance).
	 */
	tmg_addr = ioremap(hwdev->phy_addr + 0x51200, 0x100);

	hsync = mode->hsync_end - mode->hsync_start;
	hbp = mode->htotal - mode->hsync_end;
	hfp = mode->hsync_start - mode->hdisplay;
	vsync = mode->vsync_end - mode->vsync_start;
	vbp = mode->vtotal - mode->vsync_end;
	vfp = mode->vsync_start - mode->vdisplay;
	trace_drm_display_mode_info(mode);

	base = TMG_BASE_ADDR[a_crtc->dev_id];
	if (base) {
		dpu_write(hwdev, TMG_REG, base, disp_ready_man_en, 0);
		dpu_write(hwdev, TMG_REG, base, background_r, 0);
		dpu_write(hwdev, TMG_REG, base, background_g, 0x0);
		dpu_write(hwdev, TMG_REG, base, background_b, 0xff);
		dpu_write(hwdev, TMG_REG, base, eof_1st_ln_dly_num, vfp - 7);
		dpu_write(hwdev, TMG_REG, base, eof_2nd_ln_dly_num, vfp - 6);
		dpu_write(hwdev, TMG_REG, base, split_en, 0);
		dpu_write(hwdev, TMG_REG, base, cmd_screen, 0);
		dpu_write(hwdev, TMG_REG, base, cmd_wait_en, 0);
		dpu_write(hwdev, TMG_REG, base, cmd_wait_te, 0);
		dpu_write(hwdev, TMG_REG, base, sof_pre_ln_num, 0);
		dpu_write(hwdev, TMG_REG, base, hfp, hfp);
		dpu_write(hwdev, TMG_REG, base, hbp, hbp);
		dpu_write(hwdev, TMG_REG, base, vfp, vfp);
		dpu_write(hwdev, TMG_REG, base, vbp, vbp);
		dpu_write(hwdev, TMG_REG, base, hsync_width, hsync);
		dpu_write(hwdev, TMG_REG, base, vsync_width, vsync);
		dpu_write(hwdev, TMG_REG, base, hsp, 1);
		dpu_write(hwdev, TMG_REG, base, vsp, 1);
		dpu_write(hwdev, TMG_REG, base, h_active, mode->hdisplay);
		dpu_write(hwdev, TMG_REG, base, v_active, mode->vdisplay);
		dpu_write(hwdev, TMG_REG, base, fm_timing_en, 1);
		dpu_write(hwdev, TMG_REG, base, user, a_crtc->out_format);
	}

	if (a_crtc->split_en) {
		dpu_write(hwdev, TMG_REG, base, split_en, 1);
		dpu_write(hwdev, TMG_REG, base, split_overlap, 0);
		dpu_write(hwdev, TMG_REG, base, h_active, (mode->hdisplay / 2));
	}

	/* set vfp */
	value = readl(tmg_addr + 0x14);
	value &= 0xFFFF0000;
	value |= vfp;
	writel(value, tmg_addr + 0x14);

	/* set vsync align with hsync (was gated on is_edp; mainline is DP/eDP-only) */
	value = readl(tmg_addr + 0x3c);
	value |= 0x01;
	writel(value, tmg_addr + 0x3c);

	iounmap(tmg_addr);
}

static void saturn_init_regs(struct spacemit_crtc *a_crtc)
{
	u32 base = 0;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	base = CMP_BASE_ADDR[a_crtc->dev_id];
	/* set bg color to black */
	dpu_write(hwdev, CMPS_X_REG, base, bg_color_b, 0x0);
	dpu_write(hwdev, CMPS_X_REG, base, bg_color_r, 0x0);
	dpu_write(hwdev, CMPS_X_REG, base, bg_color_g, 0x0);
	dpu_write(hwdev, CMPS_X_REG, base, bg_color_a, 0xFF);
	dpu_write(hwdev, CMPS_X_REG, base, module_enable, 1);
	dpu_write(hwdev, CMPS_X_REG, base, layer00_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer01_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer02_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer03_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer04_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer05_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer06_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer07_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer08_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer09_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer10_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer11_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer12_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer13_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer14_en, 0);
	dpu_write(hwdev, CMPS_X_REG, base, layer15_en, 0);

	base = DPU_TOP_BASE_ADDR;

	base = EE_ADDR;
	dpu_write(hwdev, EE_REG, base, m_benable, 0);

	saturn_init_tmg(a_crtc);
}

static void saturn_setup_dma_top(struct spacemit_crtc *a_crtc)
{
	u32 base = DMA_TOP_BASE_ADDR;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	dpu_write(hwdev, DMA_TOP_REG, base, image_rr_ratio, 0x10);
	dpu_write(hwdev, DMA_TOP_REG, base, pixel_num_th, 4);

	//regnum: 2, offset: 0x08
	dpu_write(hwdev, DMA_TOP_REG, base, rdma_timeout_limit, 0xFFFE);
	dpu_write(hwdev, DMA_TOP_REG, base, wdma_timeout_limit, 0xFFFF);
}

static void saturn_setup_mmu_top(struct spacemit_crtc *a_crtc)
{
	unsigned int rd_outs_num = 0;
	u32 base = MMU_TOP_BASE_ADDR;
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	rd_outs_num = RD_OUTS_NUM / 2;

	/* MMU TOP init setting */
	dpu_write(hwdev, MMU_TOP_REG, base, rdma_timelimit, RDMA_TIMELIMIT);

	dpu_write(hwdev, MMU_TOP_REG, base, sram0_tlb_axi_port_sel, 0);
	dpu_write(hwdev, MMU_TOP_REG, base, sram1_tlb_axi_port_sel, 0);
	dpu_write(hwdev, MMU_TOP_REG, base, dmac0_rd_outs_num, rd_outs_num);
	dpu_write(hwdev, MMU_TOP_REG, base, dmac1_rd_outs_num, rd_outs_num);
}

void saturn_hee_irq_enable(struct spacemit_crtc *a_crtc, bool enable)
{
	u32 offset = DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_MSK;
	u32 mask;

	trace_saturn_irq_enable("irq online", enable);
	mask = saturn_hee_get_irq_bit(INT_UNDERRUN, a_crtc->dev_id) |
	       saturn_hee_get_irq_bit(INT_CFG_RDY, a_crtc->dev_id);
	saturn_enable_irq_mask(a_crtc, enable, offset, mask);
	ur_enabhee = true;
}

void saturn_hee_dpu_init(struct spacemit_crtc *a_crtc)
{
	saturn_init_regs(a_crtc);
	saturn_setup_dma_top(a_crtc);
	saturn_setup_mmu_top(a_crtc);
	saturn_hee_irq_enable(a_crtc, true);
	saturn_init_csc(a_crtc);
}

static void spacemit_set_afbc_info(struct spacemit_plane_state *spacemit_plane_state, struct spacemit_hw_device *hwdev, uint64_t modifier, u32 module_base, struct cmdlist_regs *cl_rdma)
{
	spacemit_plane_state->afbc_state = kzalloc(sizeof(struct spacemit_afbc_state), GFP_KERNEL);
	if (!spacemit_plane_state->afbc_state) {
		DRM_ERROR("Faild to set afbc plane\n");
		return;
	}

	spacemit_get_afbc_modifier(modifier, spacemit_plane_state->afbc_state);

	dpu_write(hwdev, RDMA_PATH_X_REG, module_base, fbc_tile_type, spacemit_plane_state->afbc_state->tile_type, cl_rdma, 30);
	dpu_write(hwdev, RDMA_PATH_X_REG, module_base, fbc_yuv_transform, spacemit_plane_state->afbc_state->yuv_transform, cl_rdma, 30);
	dpu_write(hwdev, RDMA_PATH_X_REG, module_base, fbc_split_mode, spacemit_plane_state->afbc_state->split_mode, cl_rdma, 30);
	dpu_write(hwdev, RDMA_PATH_X_REG, module_base, fbc_sb_layout, spacemit_plane_state->afbc_state->block_size, cl_rdma, 30);

	kfree(spacemit_plane_state->afbc_state);
}

#define CONFIG_HW_COMPOSER_LAYER(id_name, id) \
{\
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _colorkey_en, 0, cl_cmp, id * 7 + 13); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _blend_mode, blend_mode, cl_cmp, id * 7 + 13); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _blend_sel, alpha_sel, cl_cmp, id * 7 + 14); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _layer_alpha, alpha, cl_cmp, id * 7 + 14); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _alpha_ratio, 0, cl_cmp, id * 7 + 14); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _area_left, crtc_x, cl_cmp, id * 7 + 8); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _area_top, crtc_y, cl_cmp, id * 7 + 9); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _area_right, crtc_x + crtc_w - 1,cl_cmp, id * 7 + 8); \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _area_bottom, crtc_y + crtc_h - 1,cl_cmp, id * 7 + 9); \
	if (unlikely(solid_en)) { \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_color_r, solid_r, cl_cmp, id * 7 + 10); \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_color_g, solid_g, cl_cmp, id * 7 + 11); \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_color_b, solid_b, cl_cmp, id * 7 + 12); \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_color_a, solid_a, cl_cmp, id * 7 + 13); \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_en, 1, cl_cmp, id * 7 + 14); \
		DRM_DEBUG("solid_r:0x%x, solid_g:0x%x, solid_b:0x%x, solid_a:0x%x\n", \
			solid_r, solid_g, solid_b, solid_a); \
	} else { \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _solid_en, 0, cl_cmp, id * 7 + 14); \
		dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _dma_id, rdma_id, cl_cmp, id * 7 + 14); \
	} \
	dpu_write(hwdev, CMPS_X_REG, base, layer ## id_name ## _en, 1, cl_cmp, id * 7 + 14); \
}

static void saturn_write_fbcmem_regs(struct spacemit_hw_device *hwdev, struct drm_plane_state *state, u32 rdma_id,
				     u32 base, struct cmdlist_regs *cl_rdma)
{
	struct drm_crtc_state *crtc_state = state->crtc->state;
	const struct spacemit_crtc_rdma *rdmas = to_spacemit_crtc_state(crtc_state)->rdmas;
	u32 size  = rdmas[rdma_id].fbcmem.size;// / FBCMEM_UNIT;
	u32 start = rdmas[rdma_id].fbcmem.start;// / FBCMEM_UNIT;
	bool map   = rdmas[rdma_id].fbcmem.map;

	dpu_write(hwdev, RDMA_PATH_X_REG, base, value32[31], map << 28 | start << 16 | size, cl_rdma, 31);
}

static void dpu_saturn_scaler_reuse_en(struct drm_plane *plane, bool enable)
{
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	struct cmdlist_regs *cl_dpuctl = NULL;
	struct drm_plane_state *state = plane->state;
	struct spacemit_plane_state *spacemit_plane_state = to_spacemit_plane_state(state);
	u32 rdma_id = spacemit_plane_state->rdma_id;

	cl_dpuctl = alloc_cmdlist_regs(DPU_CTL_TOP_REG);
	DRM_DEBUG("silvie dpu_saturn_scaler_reuse_en rch id %d\n",rdma_id);

	switch (rdma_id){
		case DPU_RDMA0:
			dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR, nml_scl0_reuse_en, enable, cl_dpuctl, 49);
			break;
		case DPU_RDMA1:
			dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR, nml_scl1_reuse_en, enable, cl_dpuctl, 50);
			break;
		case DPU_RDMA2:
			dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR, nml_scl2_reuse_en, enable, cl_dpuctl, 51);
			break;
		case DPU_RDMA3:
			dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR, nml_scl3_reuse_en, enable, cl_dpuctl, 52);
			break;
		default:
			DRM_ERROR("Unsupported RDMA id for scaler!\n");
	}

	cmdlist_regs_packing(plane_to_cl(plane), CMDLIST_MOD_RDMA, cl_dpuctl);
	free_cmdlist_regs(cl_dpuctl);
}

void saturn_hee_plane_update_hw_channel(struct drm_plane *plane, int slice_id)
{
	struct drm_plane_state *state = plane->state;
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(state->crtc);
	struct drm_crtc_state *crtc_state = a_crtc->crtc.state;
	struct spacemit_crtc_state *spacemit_crtc_state = to_spacemit_crtc_state(crtc_state);
	struct spacemit_plane *spacemit_plane = to_spacemit_plane(plane);
	struct drm_framebuffer *fb = plane->state->fb;
	struct spacemit_plane_state *spacemit_plane_state = to_spacemit_plane_state(state);
	struct cmdlist_regs *cl_cmp = NULL;
	u32 rdma_id = spacemit_plane_state->rdma_id;
	u8 alpha = state->alpha >> 8;
	u16 pixel_alpha = state->pixel_blend_mode;
	u32 src_w, src_h, src_x, src_y;
	u32 crtc_w, crtc_h, crtc_x, crtc_y;
	u32 alpha_sel, blend_mode = 0;
	u8 uv_swap = 0;
	uint32_t fccf = fb->format->format;
	struct drm_display_mode *mode = &state->crtc->mode;

	u32 base;
	u32 val;
	bool solid_en = false;
	u32 solid_a, solid_r, solid_g, solid_b;
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	bool is_afbc = (fb->modifier > 0);
	struct cmdlist_regs *cl_rdma = a_crtc->cl_rdma;
	int i = 0;

	trace_spacemit_plane_update_hw_channel("rdma_id", rdma_id);

	src_w = state->src_w >> 16;
	src_h = state->src_h >> 16;
	src_x = state->src_x >> 16;
	src_y = state->src_y >> 16;

	crtc_w = state->crtc_w;
	crtc_h = state->crtc_h;
	crtc_x = state->crtc_x;
	crtc_y = state->crtc_y;

	DRM_DEBUG("crtc_x %u crtc_x %u\n", crtc_x, crtc_y);

	if (rdma_id == RDMA_INVALID_ID)
		solid_en = true;


	trace_dpu_plane_info(state, fb, rdma_id, alpha,
			     state->rotation, is_afbc);

	/* For solid color both src_w and src_h are 0 */
	if (solid_en == false) {
		/* Set RDMA regs */
		base = RDMA_BASE_ADDR[rdma_id];
		dpu_write(hwdev, RDMA_PATH_X_REG, base, layer_mode, is_afbc ? 1 : 0, cl_rdma, 0);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, layer_cmpsr_id, a_crtc->dev_id, cl_rdma, 0);
		if (spacemit_plane_state->is_offline) {
			dpu_write(hwdev, RDMA_PATH_X_REG, base, is_offline, 1, cl_rdma, 1);
		} else {
			dpu_write(hwdev, RDMA_PATH_X_REG, base, is_offline, 0, cl_rdma, 1);
		}
		dpu_write(hwdev, RDMA_PATH_X_REG, base, is_two_layers, 0, cl_rdma, 1);

		dpu_write(hwdev, RDMA_PATH_X_REG, base, img_width_ly0 , fb->width, cl_rdma, 16);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, img_height_ly0 , fb->height, cl_rdma, 16);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, bbox_start_x_ly0 , src_x, cl_rdma, 17);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, bbox_start_y_ly0 , src_y, cl_rdma, 17);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, bbox_end_x_ly0 , src_x + src_w - 1, cl_rdma, 18);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, bbox_end_y_ly0 , src_y + src_h - 1, cl_rdma, 18);

		saturn_write_fbcmem_regs(hwdev, state, rdma_id, base, cl_rdma);

		val = 0;
		/* setup the rotation and axis flip bits */
		if (state->rotation & DRM_MODE_ROTATE_MASK)
			val = ilog2(plane->state->rotation & DRM_MODE_ROTATE_MASK) & 0x3;
		if (state->rotation & DRM_MODE_REFLECT_MASK) {
			if (state->rotation & DRM_MODE_ROTATE_90) {
				if (state->rotation & DRM_MODE_REFLECT_X)
					val = ROT_MODE_FLIP_90;
				if (state->rotation & DRM_MODE_REFLECT_Y)
					val = ROT_MODE_MIRROR_90;
			} else {
				if (state->rotation & DRM_MODE_REFLECT_X)
					val |= ROT_MODE_MIRROR;
				if (state->rotation & DRM_MODE_REFLECT_Y)
					val |= ROT_MODE_FLIP;
			}
		}

		dpu_write(hwdev, RDMA_PATH_X_REG, base, rot_mode_ly0, val, cl_rdma, 29);
		if (fccf == DRM_FORMAT_YVYU || fccf == DRM_FORMAT_VYUY || fccf == DRM_FORMAT_NV21 || fccf == DRM_FORMAT_YVU420 || fccf == DRM_FORMAT_Q401)
			uv_swap = 1;

		dpu_write(hwdev, RDMA_PATH_X_REG, base, uv_swap, uv_swap, cl_rdma, 29);
		dpu_write(hwdev, RDMA_PATH_X_REG, base, pixel_format, spacemit_plane_state->format, cl_rdma, 29);

		if (fb->modifier)
			spacemit_set_afbc_info(spacemit_plane_state, hwdev, fb->modifier, base, cl_rdma);

		if (spacemit_plane_state->use_scl) {
			hwdev->conf_scaler_x(state, NULL);

			dpu_saturn_scaler_reuse_en(plane, true);
		} else {
			for (i = 0; i < MAX_SCALER_NUMS; i++) {
				if (spacemit_crtc_state->scl_rdma_id[i] == rdma_id) {
					dpu_saturn_scaler_reuse_en(plane, false);
					break;
				}
			}
		}
	} else {
		solid_r = (u8)spacemit_plane_state->solid_color;
		solid_g = (u8)(spacemit_plane_state->solid_color >> 8);
		solid_b = (u8)(spacemit_plane_state->solid_color >> 16);

		solid_r = (0x3ff * solid_r) / 0xff;
		solid_g = (0x3ff * solid_g) / 0xff;
		solid_b = (0x3ff * solid_b) / 0xff;

		solid_a = (u8)(spacemit_plane_state->solid_color >> 24);
		solid_r = solid_r << hwdev->solid_color_shift;
		solid_g = solid_g << hwdev->solid_color_shift;
		solid_b = solid_b << hwdev->solid_color_shift;
	}

	/* Set layer regs for blend mode */
	switch (pixel_alpha) {
		// Pixel blend mode: coverage blend
	case DRM_MODE_BLEND_COVERAGE:
		blend_mode = 0x0;
		break;
	// Pixel blend mode: premultiplied blend
	case DRM_MODE_BLEND_PREMULTI:
		blend_mode = 0x1;
		break;
	// Pixel blend mode: blend none
	case DRM_MODE_BLEND_PIXEL_NONE:
		blend_mode = 0x0;
		break;
	default:
		DRM_ERROR("Unsupported blend mode for pixel alpha!\n");
	}

	/* Set layer regs for alpha mode */
	if (state->fb->format && (state->fb->format->has_alpha) && (pixel_alpha != DRM_MODE_BLEND_PIXEL_NONE)) {
		// Exist pixel alpha if come to here.
		if (alpha != 0xff) {
			//Pixel alpha + layer alpha(combined alpha)
			alpha_sel = 0x2;
		} else {
			//Pixel alpha
			alpha_sel = 0x1;
		}
	} else {
		// None pixel alpha if come to here.
		blend_mode = 0x0;

		// Layer alpha
		alpha_sel = 0x0;
	}

	cl_cmp = alloc_cmdlist_regs(CMPS_X_REG);

	/* enable composer and bind RDMA */
	base = CMP_BASE_ADDR[a_crtc->dev_id];
	dpu_write(hwdev, CMPS_X_REG, base, dst_w, spacemit_crtc_state->post_scl_on ? spacemit_crtc_state->post_scaler_w : mode->hdisplay, cl_cmp, 1);
	dpu_write(hwdev, CMPS_X_REG, base, dst_h, spacemit_crtc_state->post_scl_on ? spacemit_crtc_state->post_scaler_h : mode->vdisplay, cl_cmp, 1);

	switch (spacemit_plane->hw_pid) {
	case 0:
		CONFIG_HW_COMPOSER_LAYER(00, 0);
		break;
	case 1:
		CONFIG_HW_COMPOSER_LAYER(01, 1);
		break;
	case 2:
		CONFIG_HW_COMPOSER_LAYER(02, 2);
		break;
	case 3:
		CONFIG_HW_COMPOSER_LAYER(03, 3);
		break;
	case 4:
		CONFIG_HW_COMPOSER_LAYER(04, 4);
		break;
	case 5:
		CONFIG_HW_COMPOSER_LAYER(05, 5);
		break;
	case 6:
		CONFIG_HW_COMPOSER_LAYER(06, 6);
		break;
	case 7:
		CONFIG_HW_COMPOSER_LAYER(07, 7);
		break;
	case 8:
		CONFIG_HW_COMPOSER_LAYER(08, 8);
		break;
	case 9:
		CONFIG_HW_COMPOSER_LAYER(09, 9);
		break;
	case 10:
		CONFIG_HW_COMPOSER_LAYER(10, 10);
		break;
	case 11:
		CONFIG_HW_COMPOSER_LAYER(11, 11);
		break;
	case 12:
		CONFIG_HW_COMPOSER_LAYER(12, 12);
		break;
	case 13:
		CONFIG_HW_COMPOSER_LAYER(13, 13);
		break;
	case 14:
		CONFIG_HW_COMPOSER_LAYER(14, 14);
		break;
	case 15:
		CONFIG_HW_COMPOSER_LAYER(15, 15);
		break;
	default:
		DRM_ERROR("%s unsupported zpos:%d\n", __func__, state->zpos);
		break;
	}
	cmdlist_regs_packing(crtc_to_cl(plane->state->crtc), CMDLIST_MOD_COMP, cl_cmp);
	free_cmdlist_regs(cl_cmp);
}

void saturn_hee_plane_disable_hw_channel(struct drm_plane *plane, struct drm_plane_state *old_state)
{
	struct spacemit_plane *p = to_spacemit_plane(plane);
	u8 dev_id = to_spacemit_crtc(old_state->crtc)->dev_id;
	u32 base = CMP_BASE_ADDR[dev_id];
	u32 rdma_id = to_spacemit_plane_state(old_state)->rdma_id;
	struct spacemit_drm_private *priv = plane->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	struct cmdlist_regs *cl_cmp = NULL;

	trace_spacemit_plane_disable_hw_channel(p->hw_pid, rdma_id);
	cl_cmp = alloc_cmdlist_regs(CMPS_X_REG);
	switch (p->hw_pid) {
	case 0:
		dpu_write(hwdev, CMPS_X_REG, base, layer00_en, 0, cl_cmp, 14);
		break;
	case 1:
		dpu_write(hwdev, CMPS_X_REG, base, layer01_en, 0, cl_cmp, 21);
		break;
	case 2:
		dpu_write(hwdev, CMPS_X_REG, base, layer02_en, 0, cl_cmp, 28);
		break;
	case 3:
		dpu_write(hwdev, CMPS_X_REG, base, layer03_en, 0, cl_cmp, 35);
		break;
	case 4:
		dpu_write(hwdev, CMPS_X_REG, base, layer04_en, 0,  cl_cmp, 42);
		break;
	case 5:
		dpu_write(hwdev, CMPS_X_REG, base, layer05_en, 0,  cl_cmp, 49);
		break;
	case 6:
		dpu_write(hwdev, CMPS_X_REG, base, layer06_en, 0,  cl_cmp, 56);
		break;
	case 7:
		dpu_write(hwdev, CMPS_X_REG, base, layer07_en, 0, cl_cmp, 63);
		break;
	case 8:
		dpu_write(hwdev, CMPS_X_REG, base, layer08_en, 0, cl_cmp, 70);
		break;
	case 9:
		dpu_write(hwdev, CMPS_X_REG, base, layer09_en, 0, cl_cmp, 77);
		break;
	case 10:
		dpu_write(hwdev, CMPS_X_REG, base, layer10_en, 0, cl_cmp, 84);
		break;
	case 11:
		dpu_write(hwdev, CMPS_X_REG, base, layer11_en, 0, cl_cmp, 91);
		break;
	case 12:
		dpu_write(hwdev, CMPS_X_REG, base, layer12_en, 0, cl_cmp, 98);
		break;
	case 13:
		dpu_write(hwdev, CMPS_X_REG, base, layer13_en, 0, cl_cmp, 105);
		break;
	case 14:
		dpu_write(hwdev, CMPS_X_REG, base, layer14_en, 0, cl_cmp, 112);
		break;
	case 15:
		dpu_write(hwdev, CMPS_X_REG, base, layer15_en, 0, cl_cmp, 119);
		break;
	default:
		DRM_ERROR("%s unsupported zpos:%d %d\n", __func__, old_state->zpos, p->hw_pid);
		break;
	}

	cmdlist_regs_packing(crtc_to_cl(old_state->crtc), CMDLIST_MOD_COMP, cl_cmp);
	free_cmdlist_regs(cl_cmp);
}


static u32 saturn_conf_dpuctrl_scaling(struct spacemit_crtc *a_crtc)
{
	struct spacemit_crtc_scaler *scaler = NULL;
	struct spacemit_crtc_state *ac = to_spacemit_crtc_state(a_crtc->crtc.state);
	u32 scl_en = 0;
	u32 i;

	for (i = 0; i < MAX_SCALER_NUMS; i++) {
		scaler = &(ac->scalers[i]);
		DRM_DEBUG("scaler%d: in_use:0x%x rdma_id:%d\n", i, scaler->in_use, scaler->rdma_id);
		trace_dpuctrl_scaling_setting(i, scaler->in_use, scaler->rdma_id);
		if (scaler->in_use)
			scl_en |= 1 << scaler->rdma_id;
	}

	DRM_DEBUG("scl_en:%d ac->post_scl_on %d\n", scl_en, ac->post_scl_on);
	trace_dpuctrl_scaling("scl_en", scl_en);
	return scl_en;
}

void saturn_hee_conf_dpuctrl(struct drm_crtc *crtc,
		    struct drm_crtc_state *old_state)
{
	u32 scl_en = 0;
	u32 rdma_en = 0;
	u32 base;
	u32 pp_base = POST_PIPE_ADDR;
	struct spacemit_drm_private *priv = crtc->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct drm_display_mode *mode = &crtc->mode;
	struct drm_crtc_state *crtc_state = a_crtc->crtc.state;
	struct spacemit_crtc_state *spacemit_crtc_state = to_spacemit_crtc_state(crtc_state);
	struct cmdlist *cl = NULL;
	struct cmdlist_regs *pp_cl = NULL;

	base = DPU_SCENE_CTL_ADDR(a_crtc->dev_id);
	rdma_en = saturn_conf_dpuctrl_rdma(a_crtc);
	scl_en = saturn_conf_dpuctrl_scaling(a_crtc);

	pp_cl = alloc_cmdlist_regs(POSTPIPE_REG);
	//postpipe should be configed no matter whether pq function is on or off
	if (a_crtc->split_en) {
		dpu_write(hwdev, POSTPIPE_REG, pp_base, value32[0], 0x20, pp_cl, 0);
	} else {
		dpu_write(hwdev, POSTPIPE_REG, pp_base, value32[0], 0, pp_cl, 0);
	}
	dpu_write(hwdev, POSTPIPE_REG, pp_base, m_inwidth, spacemit_crtc_state->post_scl_on ? spacemit_crtc_state->post_scaler_w : mode->hdisplay, pp_cl, 13);
	dpu_write(hwdev, POSTPIPE_REG, pp_base, m_inheight, spacemit_crtc_state->post_scl_on ? spacemit_crtc_state->post_scaler_h : mode->vdisplay, pp_cl, 13);

	cmdlist_regs_packing(crtc_to_cl(crtc), CMDLIST_MOD_COMP, pp_cl);
	free_cmdlist_regs(pp_cl);

	cmdlist_sort_by_group(crtc);
	cmdlist_atomic_commit(crtc, old_state);

	dpu_write(hwdev, DPU_CTL_REG, base, nml_scl_en, scl_en);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_rch_en, rdma_en);
	dpu_write(hwdev, DPU_CTL_REG, base, timing_inter0, 8);
	dpu_write(hwdev, DPU_CTL_REG, base, timing_inter1, 8);
	dpu_write(hwdev, DPU_CTL_REG, base, video_mod, 1);
	dpu_write(hwdev, DPU_CTL_REG, base, dbg_mod, 0);

	a_crtc->dpu_online_nml_scl_en = scl_en;
	a_crtc->dpu_online_nml_rch_en = rdma_en;
	a_crtc->dpu_online_nml_outctl_en = 0x1;
	dpu_write(hwdev, DPU_CTL_REG, base, nml_outctl_en, 0x1);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_frm_timing_en, 2);
	/*
	 * The VRR-VFP TMG writes here were gated on !is_edp (DSI-only).
	 * Mainline strips DSI, so the block is unreachable and removed.
	 */
	cl = &spacemit_crtc_state->cl[0];
	if (cl->va != NULL) {
		crtc_cmdlist_sort_by_group((void *)a_crtc);
		crtc_cmdlist_atomic_commit((void *)a_crtc);
	}
}


uint32_t saturn_hee_get_cfg_rdy(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	u32 base = DPU_SCENE_CTL_ADDR(a_crtc->dev_id);

	return dpu_read_reg(hwdev, DPU_CTL_REG, base, value32[2]);
}

uint32_t saturn_hee_get_int_sts(struct spacemit_hw_device *hwdev, int dev_id)
{
	if (dev_id == COMPOSER1)
		return dpu_read_reg(hwdev, DPU_INTP_REG, DPU_INT_BASE_ADDR, value32[12]);
	else if (dev_id == COMPOSER2)
		return dpu_read_reg(hwdev, DPU_INTP_REG, DPU_INT_BASE_ADDR, value32[15]);
	else
		return 0;
}

static struct spacemit_crtc_irq_bitfield {
	enum spacemit_dpu_irq irq_id;
	u32 irq_bit;
} irq_list[] = {
	[INT_UNDERRUN] = {INT_UNDERRUN, DPU_INT_FRM_TIMING_UNFLOW},
	[INT_CFG_RDY] = {INT_CFG_RDY, DPU_INT_CFG_RDY_CLR},
	[INT_VSYNC] = {INT_VSYNC, DPU_INT_FRM_TIMING_VSYNC},
	[INT_EOF] = {INT_EOF, DPU_INT_FRM_TIMING_EOF},
	[INT_REST] = {INT_REST, DPU_REST_INT_BITS},
};

static unsigned int irq_num = ARRAY_SIZE(irq_list);

uint32_t saturn_hee_get_irq_bit(enum spacemit_dpu_irq irq_id, int dev_id)
{
	if (irq_id >= irq_num)
		return 0;

	if (dev_id == COMPOSER1 || dev_id == COMPOSER2)
		return irq_list[irq_id].irq_bit;
	else
		return 0; //TODO
}

void saturn_hee_clr_int_sts(struct spacemit_crtc *a_crtc, enum spacemit_dpu_irq data, int dev_id)
{
	struct spacemit_drm_private *priv = a_crtc->crtc.dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	if (dev_id == COMPOSER1) {
		regmap_write(hwdev->regmap, DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_STS, data);
		if ((saturn_hee_get_irq_bit(INT_UNDERRUN, dev_id) & data) && ur_enabhee)
			saturn_enable_irq_mask(a_crtc, false, DPU_INT_BASE_ADDR + DPU_ONLINE_IRQ_MSK, saturn_hee_get_irq_bit(INT_UNDERRUN, dev_id));
		return;
	} else if (dev_id == COMPOSER2) {
		dpu_write_reg(hwdev, DPU_INTP_REG, DPU_INT_BASE_ADDR, value32[15], data);
		return;
	}

	if (dev_id == COMPOSER3)
		return; //TODO

}

int saturn_hee_dpu_stop_check(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	return 0;
}

void saturn_hee_dpu_disable(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	u8 dev_id = a_crtc->dev_id;
	u32 base = DPU_SCENE_CTL_ADDR(dev_id);
	u32 tmg_base = TMG_BASE_ADDR[dev_id];

	int i = 0;

	/* disable dpu all modules */
	if (tmg_base) {
		a_crtc->outctrl_reg = dpu_read_reg(hwdev, TMG_REG, tmg_base, fm_timing_en);
		dpu_write(hwdev, TMG_REG, tmg_base, fm_timing_en, 0);
	}
	dpu_write(hwdev, DPU_CTL_REG, base, nml_rch_en, 0x0);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_scl_en, 0x0);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_outctl_en, 0x0);

	dpu_write(hwdev, DPU_CTL_REG, base, nml_frm_timing_en, 0);
	for (i = 0; i < (hwdev->rdma_nums); i++) {
		dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR,
				dpu_ctl_top_reg_0[i].cmdlist_rch_en, 0);
	}
}

void saturn_hee_dpu_restart(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev)
{
	u8 dev_id = a_crtc->dev_id;
	u32 base = DPU_SCENE_CTL_ADDR(dev_id);
	u32 tmg_base = TMG_BASE_ADDR[dev_id];
	int i = 0;

	dpu_write(hwdev, TMG_REG, tmg_base, fm_timing_en, a_crtc->outctrl_reg);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_rch_en, a_crtc->dpu_online_nml_rch_en);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_scl_en, a_crtc->dpu_online_nml_scl_en);
	dpu_write(hwdev, DPU_CTL_REG, base, nml_outctl_en, a_crtc->dpu_online_nml_outctl_en);
	for (i = 0; i < (hwdev->rdma_nums); i++) {
		dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR,
				dpu_ctl_top_reg_0[i].cmdlist_rch_en, a_crtc->dpuctrl_ctl_nml_cmdlist_rch_en[i]);
	}
	dpu_write(hwdev, DPU_CTL_REG, base, nml_frm_timing_en, 2);
	dpu_write(hwdev, CMPS_X_REG, CMP_BASE_ADDR[dev_id], module_enable, 1);
}

void saturn_hee_enable_cmdlist(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, int id, bool enable)
{
	u32 val = 0;
	u32 base = 0;
	struct drm_crtc *crtc = &a_crtc->crtc;
	struct drm_crtc_state *crtc_state = crtc->state;
	struct spacemit_crtc_state *spacemit_state = to_spacemit_crtc_state(crtc_state);
	struct spacemit_crtc_rdma *rdmas = spacemit_state->rdmas;

	base = DPU_SCENE_CTL_ADDR(a_crtc->dev_id);

	dpu_write(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR,
		      dpu_ctl_top_reg_0[id].cmdlist_rch_en, enable ? 1 : 0);
	val = dpu_read_reg(hwdev, DPU_CTL_REG, base, nml_rch_vrt_reuse);
	if (enable && rdmas[id].use_cnt > 1)
		val |= (1 << id);
	else
		val &= ~(1 << id);

	dpu_write(hwdev, DPU_CTL_REG, base, nml_rch_vrt_reuse, val);
}

void saturn_hee_cfg_cmdlist(struct spacemit_hw_device *hwdev, int id, u32 chy, u32 addrl, u32 addrh)
{
	dpu_write(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_48[id].cmdlist_ch_y_first, chy);
	dpu_write(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_0[id].cmdlist_ch_start_addrl, addrl);
	dpu_write(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_16[id].cmdlist_ch_start_addrh, addrh);
}

void saturn_hee_rdma_dmmu(struct spacemit_hw_device *hwdev, u8 tbu_id, struct tbu_instance *tbu, struct drm_framebuffer *fb, u32 val, struct cmdlist_regs *cl_rdma, struct drm_plane *plane)
{
	u32 base;
	u8 rdma_id = tbu_id / 2;
	struct cmdlist_regs *cl_tbu = NULL;

	CONFIG_RDMA_ADDR_REG(hwdev, 0, rdma_id, tbu->tbu_va[0], cl_rdma);
	CONFIG_RDMA_ADDR_REG(hwdev, 1, rdma_id, tbu->tbu_va[1], cl_rdma);
	CONFIG_RDMA_ADDR_REG(hwdev, 2, rdma_id, tbu->tbu_va[2], cl_rdma);

	base = RDMA_BASE_ADDR[rdma_id];
	dpu_write(hwdev, RDMA_PATH_X_REG, base, rdma_stride0_layer0, fb->pitches[0], cl_rdma, 15);
	dpu_write(hwdev, RDMA_PATH_X_REG, base, rdma_stride1_layer0, fb->pitches[1], cl_rdma, 15);

	cl_tbu = alloc_cmdlist_regs(MMU_TBU_REG);
	base = MMU_TBU_BASE_ADDR_ARRAY[tbu_id];
	CONFIG_TBU_REGS(hwdev, base, 0, cl_tbu);
	CONFIG_TBU_REGS(hwdev, base, 1, cl_tbu);
	CONFIG_TBU_REGS(hwdev, base, 2, cl_tbu);

	dpu_write(hwdev, MMU_TBU_REG, base, value32[0], val, cl_tbu, 0);
	cmdlist_regs_packing(plane_to_cl(plane), CMDLIST_MOD_DMMU, cl_tbu);
	free_cmdlist_regs(cl_tbu);
}


static u32 saturn_get_cl_chy_addr(void)
{
	return CMDLIST_BASE_ADDR + CMDLIST_CH_Y;
}

static u32 saturn_get_cl_start_cmps_y_addr(void)
{
	return DPU_CTRL_BASE_ADDR + CMDLIST_CH_START_CMPS_Y;
}

static u32 saturn_get_cl_cfg_rdy_addr(void)
{
	return DPU_CTRL_BASE_ADDR + CMDLIST_CFG_READY;
}

int saturn_hee_get_cl_rdma_buf(struct spacemit_crtc *a_crtc)
{
	a_crtc->cl_rdma = alloc_cmdlist_regs(RDMA_PATH_X_REG);
	if (!a_crtc->cl_rdma)
		return -ENOMEM;
	else
		return 0;
}

/* cmdlist v1 support */
struct cmdlist_header {
	uint64_t next_list_addr : 39;
	// reserved
	uint32_t: 1;
	uint32_t nod_len : 16;
	/*1: the last cmdlist node, 2: pending node, otherwise 0 */
	uint32_t nod_type : 2;
	uint32_t next_nod_secu : 1;
	uint64_t wait_event_low    : 5;
	uint64_t wait_event    : 59;
	// reserved
	uint32_t: 5;
};

struct cmdlist_row {
	uint32_t module_cfg_addr   : 19;
	uint32_t module_cfg_strobe : 12;
	//the last row tag = 1, else tag = 0.
	uint32_t row_eof_tag       : 1;
	uint32_t module_regs[3];
};

#define CL_HEADER_SZ sizeof(struct cmdlist_header)
#define CL_ROW_SZ    sizeof(struct cmdlist_row)

#define CMDLIST_RDMA_CFG_RDY(ch) (1 << ch)
#define CMDLIST_PREPQ_CFG_RDY(ch) (1 << (ch + 10))
#define CMDLIST_CMPS_CFG_RDY(ch) (1 << (ch + 20))
#define CMDLIST_WB_CFG_RDY(ch) (1 << (ch + 23))

#define CMDLIST_WAIT_EVENT_BIT_RDMA_RELOAD BIT(4)
#define CMDLIST_WAIT_EVENT_BIT_PREPQ_RELOAD BIT(5)
#define CMDLIST_WAIT_EVENT_BIT_WB0_RELOAD BIT(4)
#define CMDLIST_WAIT_EVENT_BIT_WB1_RELOAD BIT(5)
#define CMDLIST_WAIT_EVENT_BIT_CMPS_RELOAD BIT(6)

void saturn_hee_cmdlist_fill_data_row(struct cmdlist *cl, u32 strobe, u32 offset, u32 value[])
{
	struct cmdlist_row *row;
	u8 i;

	row = (struct cmdlist_row *)((char *)cl->va + CL_HEADER_SZ) + cl->nod_len;
	row->module_cfg_addr = offset >> 2;
	row->module_cfg_strobe = strobe;

	for (i = 0; i < CMDLIST_ROW_REGS; i++)
		if (strobe & CMDLIST_REG_STROBE(i))
			row->module_regs[i] = value[i];

	print_row((u32 *)row);
}

void saturn_hee_cmdlist_fill_conf_row(struct cmdlist *cl, struct spacemit_hw_device *hwdev, u8 dev_id)
{
	struct cmdlist_header *header;
	struct cmdlist_row *row;
	struct spacemit_plane_state *spacemit_pstate = cl_to_spacemit_pstate(cl);
	u8 rch_id = spacemit_pstate->rdma_id;
	unsigned int zpos = spacemit_pstate->state.zpos;
	u32 size = 0, wait_event_bits = 0, cfg_rdy_bits = 0;
	int cur_crtc_y, cur_crtc_h, next_crtc_y;

	cur_crtc_y = cur_crtc_h = next_crtc_y = -1;

	header = (struct cmdlist_header *)(cl->va);
	// fill ch_y_other row;
	row = (struct cmdlist_row *)((char *)header + CL_HEADER_SZ + cl->nod_len * CL_ROW_SZ);
	if (cl->next) {

		if (dev_id == CMDLIST_CMP_INVALID) {
			trace_u64_data("rch_id", rch_id);
			row->module_cfg_addr = ((saturn_get_cl_chy_addr() + rch_id * 4) >> 2);
		} else {
			trace_u64_data("dev_id", dev_id);
			row->module_cfg_addr = ((saturn_get_cl_chy_addr() + (5 + dev_id) * 4) >> 2);
		}
		row->module_cfg_strobe = CMDLIST_REG_STROBE(0);
		if (cl->cmdlist_ch_y_other != 0xDEADBEEF)
			next_crtc_y = cl->cmdlist_ch_y_other;
		else
			next_crtc_y = cl_to_spacemit_pstate(cl->next)->state.crtc_y;
		row->module_regs[0] = next_crtc_y;
		cl->nod_len++;
		row++;
	}

	if (cl->type == CMDLIST_PLANE) {
		// fill start cmps chy row;
		if (cl->rch_start_cmps_y != 0xDEADBEEF)
			cur_crtc_y = cl->rch_start_cmps_y;
		else
			cur_crtc_y = cl_to_spacemit_pstate(cl)->state.crtc_y;
		cur_crtc_h = cl_to_spacemit_pstate(cl)->state.crtc_h;
		/* the cmdlist nodes don't belong to the same plane */
		if (next_crtc_y != -1 && cl_to_spacemit_pstate(cl->next) != spacemit_pstate &&
				(cur_crtc_y + cur_crtc_h + 24) > next_crtc_y) {
			DRM_ERROR("Invalid gap: cur_crtc_y:%d cur_crtc_h:%d next_crtc_y:%d\n",
					cur_crtc_y, cur_crtc_h, next_crtc_y);
		}
		row->module_cfg_addr = (saturn_get_cl_start_cmps_y_addr() + rch_id * 4) >> 2;
		row->module_cfg_strobe = CMDLIST_REG_STROBE(0);
		row->module_regs[0] = (cur_crtc_y >= 24) ? (cur_crtc_y - 24) : 0;
		cl->nod_len++;
		row++;
	}

	// fill last_row
	if (cl->mode_mask & CMDLIST_MOD_RDMA) {
		wait_event_bits |= CMDLIST_WAIT_EVENT_BIT_RDMA_RELOAD;
		cfg_rdy_bits |= CMDLIST_RDMA_CFG_RDY(rch_id);
	}
	if (cl->mode_mask & CMDLIST_MOD_LP) {
		wait_event_bits |= CMDLIST_WAIT_EVENT_BIT_PREPQ_RELOAD;
		cfg_rdy_bits |= CMDLIST_PREPQ_CFG_RDY(rch_id);
	}
	if (cl->mode_mask & CMDLIST_MOD_COMP) {
		wait_event_bits |= CMDLIST_WAIT_EVENT_BIT_CMPS_RELOAD;
		cfg_rdy_bits |= CMDLIST_CMPS_CFG_RDY(dev_id);
	}
	if (cl->mode_mask & CMDLIST_MOD_WB0) {
		wait_event_bits |= CMDLIST_WAIT_EVENT_BIT_WB0_RELOAD;
		cfg_rdy_bits |= CMDLIST_WB_CFG_RDY(0);
	}
	if (cl->mode_mask & CMDLIST_MOD_WB1) {
		wait_event_bits |= CMDLIST_WAIT_EVENT_BIT_WB1_RELOAD;
		cfg_rdy_bits |= CMDLIST_WB_CFG_RDY(1);
	}
	row->module_cfg_addr = (saturn_get_cl_cfg_rdy_addr()) >> 2;
	row->module_cfg_strobe = CMDLIST_REG_STROBE(0);
	row->module_regs[0] = cfg_rdy_bits;
	row->row_eof_tag = 1;
	cl->nod_len++;
	row++;

	// fill header
	if (cl->next)
		header->next_list_addr = cl->next->pa;
	else
		header->nod_type = 1;
	header->nod_len = cl->nod_len + 1; //include header
	header->wait_event_low = wait_event_bits & 0x1F; /* low 5 bits */
	header->wait_event = wait_event_bits >> 5;

	size = (u64)row - (u64)header;
	if (size > PER_CMDLIST_SIZE)
		DRM_ERROR("plane%d cmdlist occupies %d bytes!\n", zpos, size);
}

void saturn_hee_crtc_cmdlist(struct cmdlist *cl, struct spacemit_hw_device *hwdev, struct spacemit_drm_private *priv, u8 crtc_id, u8 dev_id)
{
	u32 val = 0;

	dpu_write_reg(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_48[hwdev->rdma_nums + 2 + crtc_id].cmdlist_ch_y_first, val);

	val = ((priv->cmdlist_groups[hwdev->rdma_nums + crtc_id]->pa) & CMDLIST_ADDRL_ALIGN_MASK) >> CMDLIST_ADDRL_ALIGN_BITS;
	dpu_write_reg(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_0[hwdev->rdma_nums + 2 + crtc_id].cmdlist_ch_start_addrl, val);
	val = (priv->cmdlist_groups[hwdev->rdma_nums + crtc_id]->pa) >> 32;
	dpu_write_reg(hwdev, CMDLIST_REG, CMDLIST_BASE_ADDR, cmdlist_reg_16[hwdev->rdma_nums + 2 + crtc_id].cmdlist_ch_start_addrh, val);
	dpu_write_reg(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR,
					dpu_ctl_top_reg_10[dev_id].cmdlist_cmps_top_en, 1);
	dpu_write_reg(hwdev, DPU_CTL_TOP_REG, DPU_CTRL_BASE_ADDR,
			dpu_ctl_top_reg_10[dev_id].cmdlist_cmps_other_en, 1);

	priv->cmdlist_groups[hwdev->rdma_nums + crtc_id] = NULL;
}

#define DUMP_BUF_LEN	100
void saturn_hee_cmdlist_dump_node(struct cmdlist *cl)
{
	struct cmdlist_header *header = (struct cmdlist_header *)(cl->va);
	ssize_t ret = 0;
	char tmp[DUMP_BUF_LEN] = {0x0};

	ret += sprintf(tmp + ret, "header: ");
	ret += sprintf(tmp + ret, "next_addr:0x%llx ", (unsigned long long)header->next_list_addr);
	ret += sprintf(tmp + ret, "rows:%d ", header->nod_len);
	ret += sprintf(tmp + ret, "type:%d ", header->nod_type);
	ret += sprintf(tmp + ret, "event_low:0x%llx ", (unsigned long long)header->wait_event_low);
	ret += sprintf(tmp + ret, "event:0x%llx ", (unsigned long long)header->wait_event);
	trace_plat_cmdlist_dump_node(tmp);

	/* header is already dumped*/
	for (int i = 0; i < header->nod_len - 1; i++) {
		struct cmdlist_row *row = (struct cmdlist_row *)((char *)header + CL_HEADER_SZ) + i;
		u32 addr = row->module_cfg_addr << 2;

		ret = 0;
		ret += sprintf(tmp + ret, "row[%4d:tag:%d]: ", i, row->row_eof_tag);

		for (int j = 0; j < CMDLIST_ROW_REGS; j++) {
			if (row->module_cfg_strobe & CMDLIST_REG_STROBE(j))
				ret += sprintf(tmp + ret, "[%#10x:%#10x]", addr + j * 4, row->module_regs[j]);
			else
				ret += sprintf(tmp + ret, "%#11x:%#11x", addr + j * 4, row->module_regs[j]);
		}
		trace_plat_cmdlist_dump_node(tmp);
	}
	return;
}
