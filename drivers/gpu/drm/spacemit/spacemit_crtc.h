/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#ifndef _SPACEMIT_CRTC_H_
#define _SPACEMIT_CRTC_H_

#include <linux/delay.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/regmap.h>
#include <video/videomode.h>
#include <linux/workqueue.h>

#include <drm/drm_atomic_uapi.h>
#include <drm/drm_print.h>
#include <drm/drm_crtc.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_vblank.h>
#include "spacemit_dpu_ids.h"
#include "spacemit_cmdlist.h"
#include "spacemit_drm.h"
#include <linux/spinlock.h>

#define DMA_TOP_ARB_DEBUG_INFO0_DFT	0x20003000
#define CMDLIST_CH_DBG_CH_CS_STS	0x11
#define CMDLIST_CH_DBG_CH_CS_MASK	0xff
#define CMDLIST_CH_DBG_SIZE		0x7

#define N_SCALER_MAX			5
#define N_DMA_CHANNEL_MAX		12
#define N_DMA_LAYER_MAX			16
#define N_COMPOSER_MAX			4
#define N_COMPOSER_LAYER_MAX		16
#define N_PANEL_MAX			3
#define N_OUTCTRL_MAX			3
#define N_DISPLAY_MAX			5
#define N_LUT3D_MAX			3
#define N_CMDLIST_MAX			14

#define RDMA_INVALID_ID			(~0)
#define SCALER_INVALID_ID		((u8)(~0))

#define DPU_CTRL_MAX_TIMING_INTER1	(0xf)

#define DPU_MCLK_DEFAULT	307200000
#define DPU_AXICLK_DEFAULT	409000000

#define CMDLIST_ADDRL_ALIGN_BITS		(4) /* From cmdlist_reg_0[] in CMDLIST_REG */
#define CMDLIST_ADDRL_ALIGN_MASK		((u32)(~(BIT(CMDLIST_ADDRL_ALIGN_BITS) - 1)))

#define MAX_SCALER_NUMS		4
struct spacemit_crtc_scaler {
	u32 rdma_id;
	u32 in_use;
};

enum spacemit_dpu_irq {
	INT_UNDERRUN,
	INT_CFG_RDY,
	INT_VSYNC,
	INT_EOF,
	INT_REST,
	INT_VSYNC_UPDATE,
	INT_DEBUG,
};

enum rdma_mode {
	UP_DOWN = BIT(0),
	LEFT_RIGHT = BIT(1),
};

struct spacemit_crtc_fbcmem {
	u32 start;
	u32 size;
	bool map;
};

struct spacemit_crtc_rdma {
	enum rdma_mode mode;
	struct spacemit_crtc_fbcmem fbcmem;
	bool in_use;
	u32 use_cnt;
};

struct dpu_mmu_tbl {
	u32 size;
	void *va;
	dma_addr_t pa;
};

#define MAX_WIDTH		4096
#define DPU_NO_SLICE		(0xFF)

/**
 * struct spacemit_crtc - Per-instance state for a Saturn DPU CRTC
 * @dev: Underlying platform device for this DPU instance.
 * @crtc: Embedded DRM CRTC; reachable via to_spacemit_crtc().
 * @core: Vtable of DPU pipeline operations, populated at probe.
 * @mmu_tbl: DPU-local page table mapping planes' buffers into the TBUs.
 * @dev_id: Vendor-numbered DPU instance ID (0 or 1 on K3).
 * @irq_online: Online-compose IRQ, kept for synchronize_irq().
 * @cfg_rdy_timer: Watchdog for a CFG_RDY that never arrives.
 * @is_1st_f: True until the first frame of a new modeset is transferred.
 * @first_modeset: True until the first .atomic_enable after probe.
 * @clk_ctx: Cached clock handles, enabled by dpu_enable_clocks().
 * @aclk: Requested DPU AXI clock rate (Hz).
 * @out_format: Output pixel format register cache.
 * @dpu_online_nml_rch_en: Per-frame RDMA-channel enable mask.
 * @dpu_online_nml_scl_en: Per-frame scaler enable mask.
 * @dpu_online_nml_outctl_en: Per-frame OUTCTRL enable mask.
 * @dpuctrl_ctl_nml_cmdlist_rch_en: Per-cmdlist-group RCH enable mask.
 * @mclk_reset: DPU MCLK reset line.
 * @lcd_reset: LCD-side reset line shared with the bridge.
 * @esc_reset: Escape-clock reset line.
 * @aclk_reset: AXI clock reset line.
 * @dsc_reset: DSC reset line.
 * @cl_rdma: Cmdlist register slot for RDMA programming.
 * @cl_tbu: Cmdlist register slot for TBU (DMMU) programming.
 * @split_en: Per-CRTC split-mode enable flag (dual-pipe).
 * @flip_done: Set once the DPU has committed the most recent page flip.
 * @power_on: True between atomic_enable and atomic_disable.
 * @ur_reported: True once an underrun has been logged since the last CFG_RDY.
 * @ur_mask_armed: True while clr_int_sts() may still mask the underrun IRQ.
 */
struct spacemit_crtc {
	struct device *dev;
	struct drm_crtc crtc;
	const struct dpu_core_ops *core;
	struct dpu_mmu_tbl mmu_tbl;
	int dev_id;
	int irq_online;
	struct timer_list cfg_rdy_timer;

	bool is_1st_f;
	bool first_modeset;	/* reset DPU to stop U-Boot pipeline on first atomic_enable */
	struct dpu_clk_context clk_ctx;
	u32 aclk;
	u32 out_format;
	u32 dpu_online_nml_rch_en;
	u32 dpu_online_nml_scl_en;
	u32 dpu_online_nml_outctl_en;
	u32 dpuctrl_ctl_nml_cmdlist_rch_en[N_CMDLIST_MAX];

	struct reset_control *mclk_reset;
	struct reset_control *lcd_reset;
	struct reset_control *esc_reset;
	struct reset_control *aclk_reset;
	struct reset_control *dsc_reset;

	struct cmdlist_regs *cl_rdma;
	struct cmdlist_regs *cl_tbu;

	unsigned int split_en;

	bool flip_done;

	bool power_on;

	bool ur_reported;
	bool ur_mask_armed;
};

static inline struct spacemit_crtc *to_spacemit_crtc(struct drm_crtc *crtc)
{
	return crtc ? container_of(crtc, struct spacemit_crtc, crtc) : NULL;
}

struct spacemit_hw_device;
struct dpu_core_ops {
	int (*init)(struct spacemit_crtc *a_crtc);
	void (*uninit)(struct spacemit_crtc *a_crtc);
	void (*run)(struct drm_crtc *crtc,
		    struct drm_crtc_state *old_state);
	void (*disable_vsync)(struct spacemit_crtc *a_crtc);
	void (*enable_vsync)(struct spacemit_crtc *a_crtc);
	u32 (*online_isr)(struct spacemit_crtc *a_crtc);
	int (*enable_clk)(struct spacemit_crtc *a_crtc);
	int (*disable_clk)(struct spacemit_crtc *a_crtc);
	int (*cal_layer_fbcmem_size)(struct drm_plane *plane,
				     struct drm_plane_state *state);
	int (*adjust_rdma_fbcmem)(struct spacemit_hw_device *hwdev,
				  struct spacemit_crtc_rdma *rdmas);
};

extern const struct dpu_core_ops dpu_saturn_ops;

int spacemit_crtc_run(struct drm_crtc *crtc,
		      struct drm_crtc_state *old_state);

struct spacemit_plane {
	struct drm_plane plane;
	struct spacemit_hw_device *hwdev;
	u32 hw_pid;
};

/* Only slot 0 is populated, but ceil(3840 / 512) slots keep stray indices in bounds. */
#define MAX_CL_NUM	8

/**
 * struct spacemit_plane_state - Subclassed drm_plane_state for the Saturn DPU
 * @state: Embedded drm_plane_state; reachable via to_spacemit_plane_state().
 * @rdma_id: RDMA channel feeding this plane, derived from its zpos.
 * @scaler_id: Scaler bound to @rdma_id, set even when this plane bypasses scaling.
 * @format: DPU-internal pixel-format token translated from the fourcc.
 * @use_scl: True when this plane goes through the scaler stage.
 * @right_image: True for the right half of a dual-pipe split; unused in v1.
 * @fbcmem_size: Bytes of FBC line buffer reserved for this plane.
 * @mmu_tbl: Per-plane page table populated by spacemit_dmmu_map().
 * @cl: Plane-level cmdlist pool used while atomic_update builds register batches.
 * @cur_cl: Index of the in-progress cmdlist in @cl.
 */
struct spacemit_plane_state {
	struct drm_plane_state state;
	u32 rdma_id;
	u8 scaler_id;
	u8 format;
	bool use_scl;
	bool right_image;
	u32 fbcmem_size;
	struct dpu_mmu_tbl mmu_tbl;
	struct cmdlist cl[MAX_CL_NUM];
	u8 cur_cl;
};

struct spacemit_plane *to_spacemit_plane(struct drm_plane *plane);

static inline struct
spacemit_plane_state *to_spacemit_plane_state(const struct drm_plane_state *state)
{
	return container_of(state, struct spacemit_plane_state, state);
}

/**
 * struct spacemit_crtc_state - Subclassed drm_crtc_state for the Saturn DPU
 * @base: Embedded drm_crtc_state; reachable via to_spacemit_crtc_state().
 * @scalers: Per-scaler config computed in spacemit_crtc_atomic_check_scaling().
 * @rdmas: Per-RDMA-channel descriptors, hwdev->rdma_nums entries.
 * @scl_rdma_reuse: Per-scaler flag: the RDMA can reuse the previous allocation.
 * @scl_rdma_id: Per-scaler RDMA channel binding for this commit.
 * @cl: CRTC-level cmdlist pool used during atomic_flush.
 * @cur_cl: Index of the in-progress cmdlist in @cl.
 */
struct spacemit_crtc_state {
	struct drm_crtc_state base;
	struct spacemit_crtc_scaler scalers[MAX_SCALER_NUMS];
	struct spacemit_crtc_rdma *rdmas;
	bool scl_rdma_reuse[MAX_SCALER_NUMS];
	u32 scl_rdma_id[MAX_SCALER_NUMS];
	struct cmdlist cl[MAX_CL_NUM];
	int cur_cl;
};

#define to_spacemit_crtc_state(x) container_of(x, struct spacemit_crtc_state, base)

struct drm_plane *spacemit_plane_init(struct drm_device *drm,
				      struct spacemit_crtc *a_crtc);

struct spacemit_plane_state;
struct spacemit_crtc;
struct tbu_instance;
struct dpu_mmu_tbl;
enum spacemit_dpu_irq;

/*
 * Per-revision Saturn DPU description and operation vtable. One static instance
 * per DT compatible, selected at probe; reached through spacemit_crtc::core.
 */
struct spacemit_hw_device {
	void __iomem *base;
	struct regmap *regmap;
	phys_addr_t phy_addr;
	u8 plane_nums;
	u8 rdma_nums;
	u8 crtc_nums;
	const struct spacemit_hw_rdma *rdmas;
	u8 n_formats;
	const struct dpu_format_id *formats;
	u8 n_fbcmems;
	const u32 *fbcmem_sizes;
	u32 solid_color_shift;
	u16 scaler_num;
	enum drm_color_encoding color_encoding;
	enum drm_color_range color_range;
	void (*update_csc_matrix)(struct drm_plane *plane, struct drm_plane_state *old_state);
	void (*conf_scaler_x)(struct drm_plane_state *state, struct cmdlist_regs *cl_scl);
	void (*enable_vsync)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev,
			     bool enable);
	void (*enable_cfg_irq)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev,
			       bool enable);
	void (*cfg_ready)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*sw_start)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*dpu_init)(struct spacemit_crtc *a_crtc);
	void (*irq_enable)(struct spacemit_crtc *a_crtc, bool enable);
	void (*plane_update_hw_channel)(struct drm_plane *plane, int slice_id);
	void (*plane_disable_hw_channel)(struct drm_plane *plane,
					 struct drm_plane_state *old_state);
	void (*conf_dpuctrl)(struct drm_crtc *crtc, struct drm_crtc_state *old_state);
	u32 (*get_cfg_rdy)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	u32 (*get_irq_bit)(enum spacemit_dpu_irq irq_id, int dev_id);
	u32 (*get_int_sts)(struct spacemit_hw_device *hwdev, int dev_id);
	u32 (*get_rdma_dbg_sts)(struct spacemit_crtc *a_crtc, int dev_id);
	void (*clr_int_sts)(struct spacemit_crtc *a_crtc, u32 data, int dev_id);
	void (*enable_cmdlist)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev,
			       int id, bool enable);
	void (*cfg_cmdlist)(struct spacemit_hw_device *hwdev, int id, u32 chy, u32 addrl,
			    u32 addrh);
	void (*rdma_dmmu)(struct spacemit_hw_device *hwdev, u8 tbu_id, struct tbu_instance *tbu,
			  struct drm_framebuffer *fb, u32 val, struct cmdlist_regs *cl_rdma,
			  struct drm_plane *plane);
	int (*get_cl_rdma_buf)(struct spacemit_crtc *a_crtc);
	void (*cmdlist_fill_data_row)(struct cmdlist *cl, u32 strobe, u32 offset, u32 value[]);
	void (*cmdlist_fill_conf_row)(struct cmdlist *cl, struct spacemit_hw_device *hwdev,
				      u8 dev_id);
	void (*crtc_cmdlist)(struct cmdlist *cl, struct spacemit_hw_device *hwdev,
			     struct spacemit_drm_private *priv, u8 crtc_id, u8 dev_id);
	void (*cmdlist_dump_node)(struct cmdlist *cl);
};
#endif
