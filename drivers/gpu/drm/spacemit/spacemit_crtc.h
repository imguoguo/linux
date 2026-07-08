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
#include <dt-bindings/display/spacemit_dpu.h>
#include "dpu/saturn_regs/reg_map.h"
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

#define DPU_STOP_TIMEOUT		(2000)
#define DPU_STOP_REBOOT_TIMEOUT		(150000)
#define DPU_CTRL_MAX_TIMING_INTER1	(0xf)

#define DPU_MCLK_DEFAULT	307200000
#define DPU_AXICLK_DEFAULT	409000000

#define CMDLIST_ADDRL_ALIGN_BITS		(4) //From cmdlist_reg_0[] in CMDLIST_REG
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

struct dpu_clk_context {
	struct clk *pxclk;
	struct clk *mclk;
	struct clk *escclk;
	struct clk *aclk;
	struct clk *dscclk;
};

#define MAX_WIDTH		4096
#define DPU_NO_SLICE		(0xFF)

/**
 * struct spacemit_crtc - Per-instance state for a Saturn DPU CRTC
 * @dev:              Underlying platform device for this DPU instance.
 * @crtc:             DRM CRTC embedded in this structure; reachable via
 *                    to_spacemit_crtc(crtc).
 * @core:             Vtable of DPU pipeline operations (init, enable, isr,
 *                    ...). One entry per Saturn HEE revision; populated at
 *                    probe by spacemit_crtc_init().
 * @mmu_tbl:          DPU-local page-table descriptor used by spacemit_dmmu
 *                    to map planes' DMA buffers into the DPU's TBUs.
 * @dev_id:           Vendor-numbered DPU instance ID (0 or 1 on K3).
 * @cfg_rdy_timer:    Watchdog timer that fires if CFG_RDY is not seen
 *                    within the expected vblank window. Once-shot, used
 *                    by the diagnostic path only.
 * @is_1st_f:         Per-pipeline flag, true until the first valid frame
 *                    of a new modeset has been transferred. Used by the
 *                    underrun-debug path to suppress the spurious very-
 *                    first-frame UNDERRUN on K3.
 * @first_modeset:    True until the first .atomic_enable on this CRTC
 *                    after probe; flipped false at the end of the first
 *                    enable. Drives the U-Boot pipeline reset described
 *                    in spacemit_crtc_atomic_enable() ([[k3-dpu-reset-timing]]).
 * @vrr_vfp:          Currently-programmed variable front porch. Updated
 *                    by atomic_begin so VRR mode changes take effect on
 *                    the next vblank.
 * @clk_ctx:          Cached clock handles for the DPU; prepared/enabled by
 *                    dpu_enable_clocks() during atomic_enable.
 * @aclk:             Current DPU AXI clock rate cache (Hz).
 * @escclk:           Current DSI escape clock rate cache (Hz). Dead on
 *                    K3 v1 but field is retained for hwdev symmetry.
 * @out_format:       Output pixel format register cache.
 * @stop_to:          Stop-timeout register cache (CFG_RDY watchdog).
 * @is_stopped:       True when atomic_disable has run and the DPU is
 *                    held quiescent until the next atomic_enable.
 * @outctrl_reg:      Cached DPU OUTCTRL register value.
 * @dpu_online_nml_rch_en: Per-frame RDMA-channel enable mask (online).
 * @dpu_online_nml_scl_en: Per-frame scaler enable mask (online).
 * @dpu_online_nml_outctl_en: Per-frame OUTCTRL enable mask (online).
 * @dpuctrl_ctl_nml_cmdlist_rch_en: Per-cmdlist-group RCH enable mask.
 * @mclk_reset:       DPU MCLK reset line.
 * @lcd_reset:        LCD-side reset line shared with the bridge.
 * @esc_reset:        Escape-clock reset line.
 * @aclk_reset:       AXI clock reset line.
 * @dsc_reset:        DSC reset line.
 * @expected_present_time: HWC3-style frame timing property (Android
 *                    legacy; out of scope for v1 upstream).
 * @cl_rdma:          Cmdlist register slot for RDMA programming.
 * @cl_tbu:           Cmdlist register slot for TBU (DMMU) programming.
 * @split_en:         Per-CRTC split-mode enable flag (dual-pipe).
 * @post_scaler_property: Vendor-only post-scaler control property.
 * @flip_done:        Set by the vblank handler when the most-recent
 *                    page flip has been committed by the DPU.
 * @rpm_status:       True between runtime_resume and runtime_suspend.
 * @power_on:         True between atomic_enable's power-on and
 *                    atomic_disable's power-off.
 */
struct spacemit_crtc {
	struct device *dev;
	struct drm_crtc crtc;
	const struct dpu_core_ops *core;
	struct dpu_mmu_tbl mmu_tbl;
	int dev_id;
	struct timer_list cfg_rdy_timer;

	bool is_1st_f;
	bool first_modeset;	/* reset DPU to stop U-Boot pipeline on first atomic_enable */
	uint32_t vrr_vfp;
	struct dpu_clk_context clk_ctx;
	uint32_t aclk;
	uint32_t escclk;
	uint32_t out_format;
	uint32_t stop_to;
	bool is_stopped;		/* skip dpu key regs when dpu_stopped */
	uint32_t outctrl_reg;
	uint32_t dpu_online_nml_rch_en;
	uint32_t dpu_online_nml_scl_en;
	uint32_t dpu_online_nml_outctl_en;
	uint32_t dpuctrl_ctl_nml_cmdlist_rch_en[N_CMDLIST_MAX];

	struct reset_control *mclk_reset;
	struct reset_control *lcd_reset;
	struct reset_control *esc_reset;
	struct reset_control *aclk_reset;
	struct reset_control *dsc_reset;

	// Added for Android14(HWC3.x)
	struct drm_property *expected_present_time;
	/* add for cmdlist_v1 (led) */
	struct cmdlist_regs *cl_rdma;
	struct cmdlist_regs *cl_tbu;

	unsigned int split_en;
	struct drm_property *post_scaler_property;

	bool flip_done;

	bool rpm_status;
	bool power_on;
};

static inline struct spacemit_crtc *to_spacemit_crtc(struct drm_crtc *crtc)
{
	return crtc ? container_of(crtc, struct spacemit_crtc, crtc) : NULL;
}

struct spacemit_hw_device;
struct dpu_core_ops {
	int (*parse_dt)(struct spacemit_crtc *a_crtc, struct device_node *np);
	u32 (*version)(struct spacemit_crtc *a_crtc);
	int (*init)(struct spacemit_crtc *a_crtc);
	void (*uninit)(struct spacemit_crtc *a_crtc);
	void (*run)(struct drm_crtc *crtc,
		    struct drm_crtc_state *old_state);
	void (*stop)(struct spacemit_crtc *a_crtc);
	void (*esd_restart)(struct spacemit_crtc *a_crtc);
	void (*flip)(struct spacemit_crtc *a_crtc);
	void (*disable_vsync)(struct spacemit_crtc *a_crtc);
	void (*enable_vsync)(struct spacemit_crtc *a_crtc);
	u32 (*online_isr)(struct spacemit_crtc *a_crtc);
	int (*modeset)(struct spacemit_crtc *a_crtc, struct drm_mode_modeinfo *mode);
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
	struct drm_property *rdma_id_property;
	struct drm_property *solid_color_property;
	struct drm_property *dec_lines_property;
	u32 hw_pid;
};

/*
 * Without slice/offline modes only the single per-state cmdlist
 * (index 0) is ever populated, but keep the array at MAX_WIDTH /
 * 512 = 8 slots so latent callers that index past 0 stay in bounds.
 */
#define MAX_CL_NUM	8

/**
 * struct spacemit_plane_state - Subclassed drm_plane_state for the Saturn DPU
 * @state:        Embedded drm_plane_state; reachable via
 *                to_spacemit_plane_state().
 * @rdma_id:      Index of the RDMA channel feeding this plane on the
 *                current commit. Picked in spacemit_plane_atomic_check()
 *                from the channel pool advertised by spacemit_hw_device.
 * @solid_color:  Encoded solid-color value when the plane is configured
 *                as a constant fill (no framebuffer).
 * @dec_lines:    Number of lines the FBC decoder needs to prefetch ahead
 *                of the active region for this mode.
 * @scaler_id:    Scaler instance bound to this plane's RDMA channel. Set
 *                to the scaler's id whenever the channel matches a
 *                scaler's source, even when this plane itself bypasses
 *                scaling (so other planes sharing the channel see the
 *                same binding).
 * @format:       DPU-internal pixel-format token (translated from the
 *                drm_fb fourcc by spacemit_hw_device->formats[]).
 * @use_scl:      True when this plane goes through the scaler stage.
 * @is_offline:   True when this plane is routed through an offline-mode
 *                RDMA. Dead on v1 (offline mode stripped) but retained
 *                for source-compatibility with vendor headers.
 * @right_image:  True when this plane carries the right half of a
 *                dual-pipe split. Kept for hwdev->plane_update_hw_channel()
 *                bookkeeping; v1 only uses the false branch.
 * @fbcmem_size:  Bytes of FBC line buffer reserved for this plane.
 * @mmu_tbl:     Per-plane DPU-local page-table descriptor populated by
 *                spacemit_dmmu_map().
 * @cl:          Pool of plane-level cmdlists used while atomic_update
 *                builds register batches.
 * @cur_cl:      Index of the in-progress cmdlist in @cl.
 * @afbc_state:  Vendor AFBC handoff state; dead on v1, retained for
 *                source compat.
 */
struct spacemit_plane_state {
	struct drm_plane_state state;
	u32 rdma_id;
	u32 solid_color;
	u32 dec_lines;
	u8 scaler_id;
	u8 format;
	bool use_scl;
	bool is_offline;
	bool right_image;
	u32 fbcmem_size;
	struct dpu_mmu_tbl mmu_tbl;
	struct cmdlist cl[MAX_CL_NUM];
	u8 cur_cl;
	struct spacemit_afbc_state *afbc_state;
};

struct spacemit_plane *to_spacemit_plane(struct drm_plane *plane);

static inline struct
spacemit_plane_state *to_spacemit_plane_state(const struct drm_plane_state *state)
{
	return container_of(state, struct spacemit_plane_state, state);
}

/**
 * struct spacemit_crtc_state - Subclassed drm_crtc_state for the Saturn DPU
 * @base:              Embedded drm_crtc_state; reachable via
 *                     to_spacemit_crtc_state().
 * @post_scaler_w:     Output width selected for the post-pipe scaler this
 *                     commit. 0 if scaler is bypassed.
 * @post_scaler_h:     Output height for the post-pipe scaler.
 * @post_scl_on:       True when the post-pipe scaler is engaged for this
 *                     state.
 * @scalers:           Per-scaler config (up to MAX_SCALER_NUMS) computed in
 *                     spacemit_crtc_atomic_check_scaling().
 * @rdmas:             Per-RDMA-channel descriptors allocated by
 *                     atomic_duplicate_state() and freed by
 *                     atomic_destroy_state(); length = hwdev->rdma_nums.
 * @expected_present_time: HWC3-style userspace timing hint. Out-of-scope
 *                     for v1 upstream but kept for source compatibility.
 * @scl_rdma_reuse:    Per-scaler flag indicating that the associated RDMA
 *                     can reuse a previous frame's allocation.
 * @scl_rdma_id:       Per-scaler RDMA channel binding for this commit.
 * @cl:                Pool of CRTC-level cmdlists used during atomic_flush.
 * @cur_cl:            Index of the currently-being-built cmdlist in @cl.
 */
struct spacemit_crtc_state {
	struct drm_crtc_state base;
	uint32_t post_scaler_w;
	uint32_t post_scaler_h;
	bool post_scl_on;
	struct spacemit_crtc_scaler scalers[MAX_SCALER_NUMS];
	struct spacemit_crtc_rdma *rdmas;
	u64 expected_present_time;
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
 * struct spacemit_hw_device - per-board Saturn DPU hardware description + vtable
 *
 * One static instance per Saturn DPU revision, picked up at probe via the
 * DT compatible string. Holds:
 *
 *   - The MMIO base (@base) and its phys addr for cmdlist
 *     descriptors that reference real-physical addresses;
 *   - Pipeline-size constants (@plane_nums, @rdma_nums, @scaler_num, etc.)
 *     that determine how spacemit_crtc allocates per-channel state;
 *   - The supported pixel-format table (@formats, @n_formats) and
 *     internal RDMA descriptor table (@rdmas);
 *   - FBC-memory line-buffer size table (@fbcmem_sizes, @n_fbcmems);
 *   - A vtable of Saturn-HEE-version-specific operations the CRTC and
 *     plane atomic paths dispatch through (@dpu_init, @irq_enable,
 *     @plane_update_hw_channel, @cfg_ready, @crtc_cmdlist, …).
 *
 * Each callback is documented inline above its declaration in the
 * vendor docs PDF set; for v1 upstream the goal is to expose only the
 * subset actually used by the DP/eDP primary-plane path. The MIPI-DSI,
 * writeback, slice and post-process callbacks are vestigial here (the
 * strip phase removed their implementations but kept the slots for
 * source compatibility with the dpu_saturn / dpu_saturn_hee headers).
 *
 * Fields:
 * @base / @phy_addr       — DPU register bank MMIO + phys addr.
 * @plane_nums             — Number of input planes the DPU exposes.
 * @offline_plane_nums     — Number of offline-mode planes (dead on v1).
 * @rdma_nums              — Number of RDMA channels feeding the planes.
 * @crtc_nums              — Number of CRTC pipelines on this DPU.
 * @rdmas                  — Static RDMA descriptor table.
 * @n_formats / @formats   — Pixel-format table consulted by plane init.
 * @n_fbcmems / @fbcmem_sizes — FBC line-buffer size table per RDMA.
 * @solid_color_shift      — Bit shift for the solid-color layer register.
 * @reboot_flag            — Internal reboot-bootstrap counter.
 * @scaler_num             — Number of scaler instances per pipeline.
 * @crtc_num               — Duplicate of @crtc_nums kept for symmetry
 *                              with vendor headers.
 * @color_encoding         — Default colour-encoding (BT.709 today).
 * @color_range            — Default colour-range (full-range today).
 *
 * The function-pointer members below form the Saturn-revision-specific
 * vtable. They are populated at probe by the matching dpu_saturn_*
 * compile unit (one .c per HEE revision). Calling them directly is
 * forbidden -- the CRTC/plane paths reach them through the spacemit_crtc
 * @core pointer or through spacemit_drm_private @hwdev.
 */
struct spacemit_hw_device {
	void __iomem *base;
	struct regmap *regmap;
	phys_addr_t phy_addr;
	u8 plane_nums;
	u8 offline_plane_nums;
	u8 rdma_nums;
	u8 crtc_nums;
	const struct spacemit_hw_rdma *rdmas;
	u8 n_formats;
	const struct dpu_format_id *formats;
	u8 n_fbcmems;
	const u32 *fbcmem_sizes;
	u32 solid_color_shift;
	u8 reboot_flag;
	u16 scaler_num;
	u8 crtc_num;
	enum drm_color_encoding color_encoding;
	enum drm_color_range color_range;
	void (*update_csc_matrix)(struct drm_plane *plane, struct drm_plane_state *old_state);
	void (*conf_scaler_x)(struct drm_plane_state *state, struct cmdlist_regs *cl_scl);
	void (*enable_vsync)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, bool enable);
	void (*enable_cfg_irq)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, bool enable);
	void (*cfg_ready)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*sw_start)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*dpu_init)(struct spacemit_crtc *a_crtc);
	void (*irq_enable)(struct spacemit_crtc *a_crtc, bool enable);
	void (*plane_update_hw_channel)(struct drm_plane *plane, int slice_id);
	void (*plane_disable_hw_channel)(struct drm_plane *plane, struct drm_plane_state *old_state);
	void (*conf_dpuctrl)(struct drm_crtc *crtc, struct drm_crtc_state *old_state);
	uint32_t (*get_cfg_rdy)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	uint32_t (*get_irq_bit)(enum spacemit_dpu_irq irq_id, int dev_id);
	uint32_t (*get_int_sts)(struct spacemit_hw_device *hwdev, int dev_id);
	uint32_t (*get_rdma_dbg_sts)(struct spacemit_crtc *a_crtc, int dev_id);
	void (*clr_int_sts)(struct spacemit_crtc *a_crtc, u32 data, int dev_id);
	void (*dpu_disable)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*dpu_restart)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	int (*dpu_stop_check)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev);
	void (*enable_cmdlist)(struct spacemit_crtc *a_crtc, struct spacemit_hw_device *hwdev, int id, bool enable);
	void (*cfg_cmdlist)(struct spacemit_hw_device *hwdev, int id, u32 chy, u32 addrl, u32 addrh);
	void (*rdma_dmmu)(struct spacemit_hw_device *hwdev, u8 tbu_id, struct tbu_instance *tbu, struct drm_framebuffer *fb, u32 val, struct cmdlist_regs *cl_rdma, struct drm_plane *plane);
	int (*get_cl_rdma_buf)(struct spacemit_crtc *a_crtc);
	void (*cmdlist_fill_data_row)(struct cmdlist *cl, u32 strobe, u32 offset, u32 value[]);
	void (*cmdlist_fill_conf_row)(struct cmdlist *cl, struct spacemit_hw_device *hwdev, u8 dev_id);
	void (*crtc_cmdlist)(struct cmdlist *cl, struct spacemit_hw_device *hwdev, struct spacemit_drm_private *priv, u8 crtc_id, u8 dev_id);
	void (*cmdlist_dump_node)(struct cmdlist *cl);
};
#endif
