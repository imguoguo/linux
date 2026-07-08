// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <linux/of.h>
#include <linux/bits.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/component.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/media-bus-format.h>
#include <linux/mfd/syscon.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/phy/phy-dp.h>
#include <linux/regmap.h>
#include <drm/display/drm_dp_aux_bus.h>
#include <drm/drm_of.h>
#include <drm/drm_bridge.h>
#include <drm/drm_bridge_connector.h>
#include <drm/drm_device.h>
#include <drm/drm_encoder.h>
#include <drm/drm_connector.h>
#include <drm/drm_drv.h>
#include <drm/drm_edid.h>
#include <drm/drm_modeset_helper_vtables.h>
#include <drm/drm_gem_dma_helper.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_state_helper.h>
#include <drm/display/drm_dp_aux_bus.h>
#include <drm/display/drm_dp.h>
#include <drm/display/drm_dp_helper.h>
#include <drm/drm_print.h>

#include "spacemit_inno_dp.h"

#define SOC_DP_SWING_MAX  2
#define SOC_DP_PREEMP_MAX 2
#define SOC_DP_AUX_MAX_RETRIES 3

#define SOC_DP_EDID_CHUNK_SIZE 16
#define SOC_DP_DDC_SEGMENT_ADDR 0x30
#define SOC_DP_SINK_READY_DELAY_MS 120
#define SOC_DP_SINK_READY_RETRIES 3

#define SOC_DP_QOS_MUX_CTRL	0x12c

/**
 * struct soc_dp_chip_data - per-instance configuration looked up by MMIO base.
 * @reg_base: physical base address of the DP controller register block; used
 *	as the lookup key when the platform device probes.
 * @dpu_id: which DPU instance feeds this DP controller. Determines the
 *	QoS-mux routing.
 * @edp_mode: true when the downstream port is the eDP panel path; false for
 *	the external DP connector path. Drives bridge type / ops selection
 *	(eDP omits OP_DETECT and OP_MODES).
 *
 * The K3 SoC instantiates the same Innosilicon DP IP twice -- eDP0 at
 * 0xcac84000 and DP1 at 0xcac88000 -- so the controller-level binding uses
 * a single compatible and the per-instance attributes are derived here from
 * the platform_get_resource() MEM base.
 */
struct soc_dp_chip_data {
	resource_size_t reg_base;
	u8 dpu_id;
	bool edp_mode;
};

static const struct soc_dp_chip_data soc_dp_chip_table[] = {
	{ .reg_base = 0xcac84000, .dpu_id = 0, .edp_mode = true  }, /* eDP0 */
	{ .reg_base = 0xcac88000, .dpu_id = 1, .edp_mode = false }, /* DP1  */
};

static const struct soc_dp_chip_data *
soc_dp_match_chip(resource_size_t base)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(soc_dp_chip_table); i++) {
		if (soc_dp_chip_table[i].reg_base == base)
			return &soc_dp_chip_table[i];
	}
	return NULL;
}

#include <linux/clk.h>
#include <linux/math64.h>
#include <linux/reset.h>

/*
 * Local definitions for Link Configuration.
 * Decoupled from <drm/drm_dp_helper.h> to facilitate bare-metal porting.
 */
enum soc_dp_link_rate {
	SOC_DP_LINK_RATE_1_62 = 1620000, /* 1.62 Gbps */
	SOC_DP_LINK_RATE_2_70 = 2700000, /* 2.70 Gbps */
	SOC_DP_LINK_RATE_5_40 = 5400000, /* 5.40 Gbps */
	SOC_DP_LINK_RATE_8_10 = 8100000, /* 8.10 Gbps */
};

enum soc_dp_lane_count {
	SOC_DP_LANE_1 = 1,
	SOC_DP_LANE_2 = 2,
	SOC_DP_LANE_4 = 4,
};

enum soc_video_format {
	SOC_VIDEO_RGB_6BIT = 0,
	SOC_VIDEO_RGB_8BIT = 1,
	SOC_VIDEO_RGB_10BIT = 2,
	SOC_VIDEO_RGB_12BIT = 3,
	SOC_VIDEO_RGB_16BIT = 4,
	SOC_VIDEO_YUV444_8BIT = 5,
	SOC_VIDEO_YUV444_10BIT = 6,
	SOC_VIDEO_YUV444_12BIT = 7,
	SOC_VIDEO_YUV444_16BIT = 8,
	SOC_VIDEO_YUV422_8BIT = 9,
	SOC_VIDEO_YUV422_10BIT = 10,
	SOC_VIDEO_YUV422_12BIT = 11,
	SOC_VIDEO_YUV422_16BIT = 12,
};

enum soc_dp_ref_clk {
	SOC_DP_REF_CLK_24M = 24000,
	SOC_DP_REF_CLK_50M = 50000,
};

static const struct soc_dp_link_config {
	enum soc_dp_link_rate rate;
	enum soc_dp_lane_count lanes;
} soc_dp_link_priority_table[] = {
	/* --- Tier 1: Low Bandwidth (< 4 Gbps) --- */
	{SOC_DP_LINK_RATE_1_62, SOC_DP_LANE_1}, /* 1.62 Gbps */
	{SOC_DP_LINK_RATE_2_70, SOC_DP_LANE_1}, /* 2.70 Gbps */
	{SOC_DP_LINK_RATE_1_62, SOC_DP_LANE_2}, /* 3.24 Gbps */

	/* --- Tier 2: Medium Bandwidth (~5-6 Gbps) --- */
	{SOC_DP_LINK_RATE_2_70, SOC_DP_LANE_2}, /* 5.40 Gbps */
	{SOC_DP_LINK_RATE_1_62, SOC_DP_LANE_4}, /* 6.48 Gbps */

	/* --- Tier 3: High Bandwidth (~10 Gbps) --- */
	{SOC_DP_LINK_RATE_2_70, SOC_DP_LANE_4}, /* 10.8 Gbps */
	{SOC_DP_LINK_RATE_5_40, SOC_DP_LANE_2}, /* 10.8 Gbps */

	/* --- Tier 4: Ultra High Bandwidth (> 17 Gbps) --- */
	{SOC_DP_LINK_RATE_5_40, SOC_DP_LANE_4}, /* 21.6 Gbps */
};

static const struct soc_format_info {
	uint8_t bpp; /* Bits Per Pixel */
} format_info_table[] = {
	[SOC_VIDEO_RGB_6BIT]      = { .bpp = 18 },
	[SOC_VIDEO_RGB_8BIT]      = { .bpp = 24 },
	[SOC_VIDEO_RGB_10BIT]     = { .bpp = 30 },
	[SOC_VIDEO_RGB_12BIT]     = { .bpp = 36 },
	[SOC_VIDEO_RGB_16BIT]     = { .bpp = 48 },

	[SOC_VIDEO_YUV444_8BIT]   = { .bpp = 24 },
	[SOC_VIDEO_YUV444_10BIT]  = { .bpp = 30 },
	[SOC_VIDEO_YUV444_12BIT]  = { .bpp = 36 },
	[SOC_VIDEO_YUV444_16BIT]  = { .bpp = 48 },

	[SOC_VIDEO_YUV422_8BIT]   = { .bpp = 16 },
	[SOC_VIDEO_YUV422_10BIT]  = { .bpp = 20 },
	[SOC_VIDEO_YUV422_12BIT]  = { .bpp = 24 },
	[SOC_VIDEO_YUV422_16BIT]  = { .bpp = 32 },
};

static int soc_dp_get_bpp(uint32_t format)
{
	if (format >= ARRAY_SIZE(format_info_table)) {
		pr_warn("DP: Invalid color format index %d, defaulting to RGB888\n", format);
		return 24;
	}
	return format_info_table[format].bpp;
}

#define SOC_DP_VCO_MIN_KHZ        1000000
#define SOC_DP_VCO_MAX_KHZ        3000000
#define SOC_DP_PLL_FRAC_MOD       16777216  /* 2^24 */
#define SOC_DP_PLL_ERR_TOLERANCE  10

/* Data structure for Core PLL results */
struct soc_dp_core_pll_cfg {
	uint32_t target_rate_kbps;
	uint32_t vco_freq_khz;
	uint8_t prediv;
	uint16_t fbdiv;
	uint32_t frac;
	uint8_t postdiv_reg;
	uint8_t frac_pd;
	uint8_t vcoclk_div8_en;
	uint8_t postdiv_en;
	uint8_t clk_16mdiv;
	uint32_t actual_rate_khz;
	bool valid;
};

static struct soc_dp_core_pll_cfg core_pll_cfg_table[] = {
	/* LinkRate 1.62Gbps */
	{
		.target_rate_kbps = 1620000,
		.vco_freq_khz     = 1620000,
		.prediv           = 0x02,
		.fbdiv            = 0x87,
		.frac             = 0x0,
		.postdiv_reg      = 0x0,
		.frac_pd          = 0x3,
		.vcoclk_div8_en   = 0x1,
		.postdiv_en       = 0x1,
		.clk_16mdiv       = 12,
		.actual_rate_khz  = 1620000,
		.valid            = true,
	},
	/* LinkRate 2.7Gbps */
	{
		.target_rate_kbps = 2700000,
		.vco_freq_khz     = 2700000,
		.prediv           = 0x02,
		.fbdiv            = 0xe1,
		.frac             = 0x0,
		.postdiv_reg      = 0x0,
		.frac_pd          = 0x3,
		.vcoclk_div8_en   = 0x1,
		.postdiv_en       = 0x1,
		.clk_16mdiv       = 21,
		.actual_rate_khz  = 2700000,
		.valid            = true,
	},
	/* LinkRate 5.4Gbps */
	{
		.target_rate_kbps = 5400000,
		.vco_freq_khz     = 2700000,
		.prediv           = 0x02,
		.fbdiv            = 0xe1,
		.frac             = 0x0,
		.postdiv_reg      = 0x0,
		.frac_pd          = 0x3,
		.vcoclk_div8_en   = 0x0,
		.postdiv_en       = 0x0,
		.clk_16mdiv       = 42,
		.actual_rate_khz  = 5400000,
		.valid            = true,
	},
};

/* Data structure for Pixel PLL results */
struct soc_dp_pixel_pll_cfg {
	uint32_t target_pclk_khz;
	uint32_t vco_freq_khz;
	uint8_t prediv;
	uint16_t fbdiv;
	uint32_t frac_pd;
	uint32_t frac;
	uint8_t div5_en;
	uint8_t divm;
	uint8_t divaux;
	uint8_t divp;
	uint32_t actual_pclk_khz;
	bool valid;
};

static const struct soc_dp_pixel_pll_cfg pixel_pll_cfg_table[] = {
	{ 614400, 2460000, 0x05, 512,  0x3, 0x0, 0x0, 0x1, 0x01, 0x1, 614400, true },
	{ 594000, 2376000, 0x01, 99,   0x3, 0x0, 0x0, 0x1, 0x01, 0x1, 594000, true },
	{ 551040, 2760000, 0x05, 574,  0x3, 0x0, 0x1, 0x0, 0x00, 0x0, 551040, true },
	{ 533250, 2130000, 0x08, 711,  0x3, 0x0, 0x0, 0x1, 0x01, 0x1, 533250, true },
	{ 443250, 1770000, 0x08, 591,  0x3, 0x0, 0x0, 0x1, 0x01, 0x1, 443250, true },
	{ 375000, 3000000, 0x01, 125,  0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 375000, true },
	{ 372000, 2980000, 0x01, 124,  0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 372000, true },
	{ 348500, 2790000, 0x06, 697,  0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 348500, true },
	{ 307200, 1540000, 0x01, 64,   0x3, 0x0, 0x1, 0x0, 0x00, 0x0, 307200, true },
	{ 297000, 2376000, 0x01, 99,   0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 297000, true },
	{ 280000, 1676000, 0x01, 70,   0x3, 0x0, 0x0, 0xa, 0x01, 0x1, 280000, true },
	{ 277440, 2770000, 0x05, 578,  0x3, 0x0, 0x0, 0x3, 0x01, 0x1, 277440, true },
	{ 245760, 2457600, 0x05, 512,  0x3, 0x0, 0x0, 0x3, 0x01, 0x1, 245760, true },
	{ 241500, 1932000, 0x02, 161,  0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 241500, true },
	{ 236000, 2830000, 0x01, 118,  0x3, 0x0, 0x0, 0x0, 0x06, 0x1, 236000, true },
	{ 204800, 2048000, 0x03, 256,  0x3, 0x0, 0x0, 0x3, 0x01, 0x1, 204800, true },
	{ 193250, 2320000, 0x08, 773,  0x3, 0x0, 0x0, 0x0, 0x06, 0x1, 193250, true },
	{ 189000, 1510000, 0x01,  63,  0x3, 0x0, 0x0, 0x0, 0x04, 0x1, 189000, true },
	{ 187500, 3000000, 0x01, 125,  0x3, 0x0, 0x0, 0x0, 0x08, 0x1, 187500, true },
	{ 162000, 2592000, 0x01, 108,  0x3, 0x0, 0x0, 0x0, 0x08, 0x1, 162000, true },
	{ 156000, 2810000, 0x01, 117,  0x3, 0x0, 0x0, 0x0, 0x09, 0x1, 156000, true },
	{ 150000, 3000000, 0x01, 125,  0x3, 0x0, 0x0, 0x0, 0x0a, 0x1, 150000, true },
	{ 148500, 2376000, 0x01, 99,   0x3, 0x0, 0x0, 0x0, 0x08, 0x1, 148500, true },
	{ 146000, 1750000, 0x01, 73,   0x3, 0x0, 0x0, 0x0, 0x06, 0x1, 146000, true },
	{ 142860, 2860000, 0x14, 2381, 0x3, 0x0, 0x0, 0x0, 0x0a, 0x1, 142860, true },
	{ 140000, 2520000, 0x01, 105,  0x3, 0x0, 0x0, 0x0, 0x09, 0x1, 140000, true },
	{ 138500, 2220000, 0x03, 277,  0x3, 0x0, 0x0, 0x0, 0x08, 0x1, 138500, true },
	{ 122000, 2930000, 0x01, 122,  0x3, 0x0, 0x0, 0x0, 0x0c, 0x1, 122000, true },
	{ 121750, 2920000, 0x04, 487,  0x3, 0x0, 0x0, 0x0, 0x0c, 0x1, 121750, true },
	{ 108000, 2810000, 0x01, 117,  0x3, 0x0, 0x0, 0x0, 0x0d, 0x1, 108000, true },
	{ 106500, 1700000, 0x01,  71,  0x3, 0x0, 0x0, 0x0, 0x08, 0x1, 106500, true },
	{ 83500,  2000000, 0x02, 167,  0x3, 0x0, 0x0, 0x0, 0x0c, 0x1, 83500,  true },
	{ 79500,  2540000, 0x01, 106,  0x3, 0x0, 0x0, 0x0, 0x10, 0x1, 79500,  true },
	{ 75000,  3000000, 0x01, 125,  0x3, 0x0, 0x0, 0x0, 0x14, 0x1, 75000,  true },
	{ 74250,  2376000, 0x01,  99,  0x3, 0x0, 0x0, 0x0, 0x10, 0x1, 74250,  true },
	{ 65000,  1560000, 0x01,  65,  0x3, 0x0, 0x0, 0x0, 0x0c, 0x1, 65000,  true },
	{ 40000,  2880000, 0x01, 120,  0x3, 0x0, 0x0, 0x0, 0x12, 0x2, 40000,  true },
	{ 27000,  2810000, 0x01, 117,  0x3, 0x0, 0x0, 0x0, 0x1a, 0x2, 27000,  true },
	{ 25600,  2300000, 0x01,  96,  0x3, 0x0, 0x0, 0x0, 0x0f, 0x3, 25600,  true },
	{ 25200,  2520000, 0x01, 105,  0x3, 0x0, 0x0, 0x0, 0x19, 0x2, 25200,  true },
};

/**
 * struct soc_dp_dev - Per-instance state for an Innosilicon DP/eDP controller
 * @dev:           Underlying platform device.
 * @drm:           Parent DRM device set at component bind time.
 * @encoder:       Passive DRM encoder owning possible_crtcs. The atomic
 *                 dispatch path goes through @bridge instead; @encoder is
 *                 just the hook drm_bridge_attach() needs.
 * @bridge:        DRM bridge that implements atomic_enable / atomic_disable /
 *                 atomic_check / mode_valid / detect / edid_read for this DP
 *                 or eDP controller. Allocated via devm_drm_bridge_alloc()
 *                 and attached to the encoder with
 *                 DRM_BRIDGE_ATTACH_NO_CONNECTOR; the connector is then
 *                 created from it by drm_bridge_connector_init().
 * @connector:     drmm-owned DRM connector built on top of @bridge by
 *                 drm_bridge_connector_init(); freed when @drm is released.
 * @connector_status: Last-known connector status driven by the HPD ISR and
 *                 the optional polling thread.
 * @mode:          Cached display mode set at .mode_set time, replayed by the
 *                 enable callback. Field will go away once atomic_check
 *                 populates the bridge state with the negotiated parameters.
 * @regs:          Mapped MMIO base for the DP controller register block.
 * @qos:           QoS syscon regmap, used to programme DP traffic priority.
 * @aux:           DPCD AUX channel (drm_dp_aux). Registered with
 *                 drm_dp_aux_register() at bind.
 * @dpcd:          Raw DPCD capability bytes read by soc_dp_hw_read_sink_caps()
 *                 via drm_dp_read_dpcd_caps().
 * @lane_count:    Final lane count chosen by link training (1, 2 or 4).
 * @link_rate:     Final link rate chosen by link training, in kHz
 *                 (e.g. 2700000 for HBR @ 2.7 GHz).
 * @link:          Sink-advertised link capability snapshot, populated by
 *                 soc_dp_hw_read_sink_caps() and consulted by mode_set when
 *                 selecting a rate/lane combination.
 * @link.revision: DPCD revision byte (DP_DPCD_REV).
 * @link.enhanced_framing: Whether the sink advertises enhanced framing.
 * @link.max_rate: Sink-advertised maximum link rate, kHz.
 * @link.max_num_lanes: Sink-advertised maximum lane count.
 * @reset:         Optional shared reset line; firmware leaves the IP
 *                 de-asserted, so this is held for completeness only.
 * @pxclk:         Optional external pixel clock. When present and selected
 *                 via use_ext_pixel_clock, mode_set programmes this directly
 *                 instead of going through the DP PLL.
 * @phy:           DP PHY handle obtained via devm_phy_get(); the PHY driver
 *                 lives at drivers/phy/spacemit/phy-spacemit-k3-inno-dp.c.
 * @phy_powered:   Controller-side mirror of the PHY framework's power
 *                 refcount; see soc_dp_phy_on() / soc_dp_phy_off().
 * @next_bridge:   Downstream bridge in the encoder-to-panel chain. On eDP
 *                 this is panel-edp's drm_panel, wrapped into a drm_bridge
 *                 by the DRM AUX-bus framework when the panel's
 *                 done_probing callback fires.
 * @edp_mode:      True when this instance drives an embedded panel (eDP);
 *                 false for external DisplayPort.
 * @dpu_id:        Which DPU instance feeds this controller. 0 = DPU0/eDP,
 *                 1 = DPU1/DP per the K3 dtsi.
 * @use_ext_pixel_clock: True when the pixel clock is sourced externally
 *                 (eDP path) rather than from the DP PLL (DP path).
 * @pixel_clock:   Cached pixel-clock rate in kHz when an external pxclk is
 *                 in use.
 * @mode_lock:     Serialises mode_set / enable / hotplug paths against
 *                 each other; protects @mode, @connector_status and link
 *                 training state.
 * @suspended:     True between runtime_suspend and the next runtime_resume;
 *                 HPD handling skips HW access while set.
 * @ref_clk:       PLL reference clock rate in Hz, used by
 *                 soc_dp_calc_core_pll_to_reg() when programming the DP PLL.
 */
struct soc_dp_dev {
	struct device *dev;

	struct drm_device *drm;
	struct drm_encoder encoder;
	struct drm_bridge bridge;
	struct drm_connector *connector;

	enum drm_connector_status connector_status;
	struct drm_display_mode mode;

	struct regmap *regs;
	struct regmap *qos;
	struct drm_dp_aux aux;

	/* Buffer to store raw DPCD data */
	uint8_t dpcd[DP_RECEIVER_CAP_SIZE];
	int lane_count;
	uint32_t link_rate;

	/* Structure to store negotiated link parameters */
	struct {
		uint8_t revision;
		uint8_t enhanced_framing;
		uint32_t max_rate;
		uint32_t max_num_lanes;
	} link;

	struct reset_control *reset;
	struct clk *pxclk;
	struct phy *phy;
	bool phy_powered;
	struct drm_bridge *next_bridge;

	bool edp_mode;
	int dpu_id;
	bool use_ext_pixel_clock;
	int pixel_clock;
	struct mutex mode_lock;
	bool suspended;

	uint32_t ref_clk;

	int irq;
};

/**
 * struct soc_dp_bridge_state - Per-state planned link configuration.
 * @base: parent &struct drm_bridge_state.
 * @link_cfg_idx: starting index into @soc_dp_link_priority_table picked by
 *	->atomic_check() based on source / sink caps and the requested mode's
 *	bandwidth. -1 means no selection has been made yet (atomic_reset
 *	default); in that case ->atomic_enable() falls back to walking the
 *	table from the start.
 * @color_format: output colour-format encoding selected by ->atomic_check()
 *	for the next commit (one of the SOC_VIDEO_* enums). Lives on the
 *	bridge state so that ->atomic_duplicate_state() and
 *	->atomic_destroy_state() see it as part of the swap; v1 only
 *	advertises SOC_VIDEO_RGB_8BIT so this is effectively a constant for
 *	now, but the slot is here so adding YCbCr later does not require
 *	reshaping the state again.
 */
struct soc_dp_bridge_state {
	struct drm_bridge_state base;
	int link_cfg_idx;
	uint32_t color_format;
};

static inline struct soc_dp_bridge_state *
to_soc_dp_bridge_state(struct drm_bridge_state *state)
{
	return container_of(state, struct soc_dp_bridge_state, base);
}

static const struct regmap_config soc_dp_regmap_config = {
	.name		= "dp",
	.reg_bits	= 32,
	.val_bits	= 32,
	.reg_stride	= 4,
	.max_register	= 0x3ffc,	/* full 0x4000 reg region from binding */
};

static int soc_dp_reg_write_range(struct soc_dp_dev *dp,
		uint32_t offset, uint32_t high, uint32_t low, uint32_t val)
{
	uint32_t mask = GENMASK(high, low);

	/*
	 * regmap_write_bits() is the force-write variant: it always issues
	 * the write, matching the original writel-based RMW. The cheaper
	 * regmap_update_bits() would skip writes whose masked target value
	 * already equals what was just read back, which breaks
	 * write-1-to-clear registers like SOC_DPTX_AUX_REPLY_EVENT_INT_STA
	 * (the AUX completion-pending bit reads 1, we want to write 1 to
	 * clear it, but update_bits sees "no change" and skips the write).
	 */
	return regmap_write_bits(dp->regs, offset, mask, (val << low) & mask);
}

static int soc_dp_reg_only_write_range(struct soc_dp_dev *dp,
		uint32_t offset, uint32_t high, uint32_t low, uint32_t val)
{
	uint32_t mask = GENMASK(high, low);

	return regmap_write(dp->regs, offset, (val << low) & mask);
}

static int soc_dp_reg_read_range(struct soc_dp_dev *dp,
		uint32_t offset, uint32_t high, uint32_t low, uint32_t *val)
{
	uint32_t mask = GENMASK(high, low);
	int ret;

	ret = regmap_read(dp->regs, offset, val);
	if (ret)
		return ret;
	*val = (*val & mask) >> low;
	return 0;
}

static uint32_t soc_dp_aux_get_cmd(struct drm_dp_aux_msg *msg)
{
	switch (msg->request & ~DP_AUX_I2C_MOT) {
	case DP_AUX_NATIVE_WRITE:
	case DP_AUX_I2C_WRITE:
	case DP_AUX_I2C_WRITE_STATUS_UPDATE:
		return msg->request;
	case DP_AUX_NATIVE_READ:
	case DP_AUX_I2C_READ:
		return msg->request;
	default:
		return 0;
	}

	return 0;
}

static void soc_dp_aux_hw_reset(struct soc_dp_dev *dp)
{
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x1);
	usleep_range(2000, 2500);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x0);
	usleep_range(2000, 2500);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_REPLY_EVENT_INT_STA, 1);
}

static bool soc_dp_dpcd_caps_valid(const u8 *dpcd)
{
	u8 max_bw = dpcd[DP_MAX_LINK_RATE];
	u8 max_lanes = dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;

	if (!dpcd[DP_DPCD_REV])
		return false;

	switch (max_bw) {
	case DP_LINK_BW_1_62:
	case DP_LINK_BW_2_7:
	case DP_LINK_BW_5_4:
	case DP_LINK_BW_8_1:
		break;
	default:
		return false;
	}

	return max_lanes == 1 || max_lanes == 2 || max_lanes == 4;
}

static ssize_t soc_dp_aux_transfer(struct drm_dp_aux *aux,
				   struct drm_dp_aux_msg *msg)
{
	int ret, i, retries = 0;
	unsigned long timeout;

	struct soc_dp_dev *dp = container_of(aux, struct soc_dp_dev, aux);
	uint32_t cmd, len, val, status;
	uint32_t data[4] = {0};
	uint8_t *buf = msg->buffer;
	bool is_read = (msg->request & DP_AUX_I2C_READ) ||
		((msg->request & DP_AUX_NATIVE_READ) == DP_AUX_NATIVE_READ);

	/* 1. Check message validity */
	if (msg->size > 16)
		return -EINVAL;

	cmd = soc_dp_aux_get_cmd(msg);

retry_eio:
	/* 2. Prepare Data for Write (if applicable) */
	if (!is_read) {
		/* Pack bytes into 32-bit words (Little Endian packing) */
		for (i = 0; i < msg->size; i++) {
			data[i / 4] |= buf[i] << ((i % 4) * 8);
		}

		/* Write data to registers: DATA1(LSB)..DATA4(MSB) */
		soc_dp_reg_write_range(dp, SOC_DPTX_AUX_DATA1, data[0]);
		soc_dp_reg_write_range(dp, SOC_DPTX_AUX_DATA2, data[1]);
		soc_dp_reg_write_range(dp, SOC_DPTX_AUX_DATA3, data[2]);
		soc_dp_reg_write_range(dp, SOC_DPTX_AUX_DATA4, data[3]);
	}

	/* 3. Configure Command, Address, Length */
	/* HW expects Length - 1 */
	len = msg->size > 0 ? msg->size - 1 : 0;

	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_LENGTH, len);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_ADDR, msg->address);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_CMD_TYPE, cmd);

	/* 4. Trigger Transfer */
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_START, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_START, 1);

	/* 5. Wait for Completion */
	timeout = jiffies + msecs_to_jiffies(200);
	ret = -ETIMEDOUT;

	while (1) {
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_REPLY_EVENT_INT_STA, &val);
		if (val) {
			ret = 0;
			break;
		}
		if (time_after(jiffies, timeout))
			break;
		usleep_range(100, 110);
	}

	if (ret) {
		if (retries < SOC_DP_AUX_MAX_RETRIES) {
			retries++;
			soc_dp_aux_hw_reset(dp);
			dev_dbg(dp->dev,
				"AUX timeout retry, cmd: 0x%x, addr: 0x%x, size %zu, retries %d/%d\n",
				cmd, msg->address, msg->size, retries, SOC_DP_AUX_MAX_RETRIES);
			goto retry_eio;
		}

		dev_err(dp->dev,
			"AUX transfer timeout, req: 0x%x, cmd: 0x%x, addr: 0x%x, size %zu\n",
			msg->request, cmd, msg->address, msg->size);
		return ret;
	}

	/* 6. Read Status */
	soc_dp_reg_read_range(dp, SOC_DPTX_AUX_STATUS, &status);

	/* 7. Clear Interrupt Status (W1C) */
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_REPLY_EVENT_INT_STA, 1);

	/* Map HW status to DRM reply codes */
	switch (status) {
	case 0: /* ACK */
		msg->reply = DP_AUX_NATIVE_REPLY_ACK;
		break;
	case 1: /* NACK */
		msg->reply = DP_AUX_NATIVE_REPLY_NACK;
		return 0; /* Standard says return 0 on NACK for upper layer retry */
	case 2: /* DEFER */
		msg->reply = DP_AUX_NATIVE_REPLY_DEFER;
		return 0;
	default:
		/* Check error code if status is weird */
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_REPLY_ERR_CODE, &val);
		if (retries < SOC_DP_AUX_MAX_RETRIES) {
			retries++;
			dev_dbg(dp->dev,
				"AUX status retry, cmd: 0x%x, addr: 0x%x, size %zu, retries %d/%d\n",
				cmd, msg->address, msg->size, retries, SOC_DP_AUX_MAX_RETRIES);
			usleep_range(1000, 1100);
			goto retry_eio;
		}
		dev_err(dp->dev, "AUX status error, cmd: 0x%x, address: 0x%x, size %zu, status: 0x%x, code: 0x%x\n",
			cmd, msg->address, msg->size, status, val);
		return -EIO;
	}

	/* 8. Read Data (if Read operation and ACK) */
	if (is_read && msg->size > 0) {
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_DATA1, &data[0]);
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_DATA2, &data[1]);
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_DATA3, &data[2]);
		soc_dp_reg_read_range(dp, SOC_DPTX_AUX_DATA4, &data[3]);

		/* Unpack 32-bit words back to bytes */
		for (i = 0; i < msg->size; i++) {
			buf[i] = (data[i / 4] >> ((i % 4) * 8)) & 0xFF;
		}
	}

	return msg->size;
}

static int soc_dp_aux_init(struct soc_dp_dev *dp)
{
	DRM_INFO("%s() \n", __func__);

	dp->aux.name = "soc-dp-aux";
	dp->aux.dev = dp->dev;
	dp->aux.drm_dev = dp->drm;
	dp->aux.transfer = soc_dp_aux_transfer;
	dp->aux.no_zero_sized = true;

	return drm_dp_aux_register(&dp->aux);
}

static uint64_t soc_dp_abs_diff(uint64_t a, uint64_t b)
{
	return (a > b) ? (a - b) : (b - a);
}

static uint32_t soc_dp_div64(uint64_t *n, uint32_t base)
{
	return do_div(*n, base);
}

static uint32_t soc_dp_get_rate_khz(uint8_t pre, uint16_t fb, uint32_t frac,
				    uint32_t ref_clk_khz, uint32_t total_div)
{
	uint64_t vco_hz;
	uint64_t ref_hz = (uint64_t)ref_clk_khz * 1000;
	uint64_t int_part;
	uint64_t frac_part;

	/* Integer part: (Ref * FB) / Pre */
	int_part = ref_hz * fb;
	int_part += (pre / 2);
	soc_dp_div64(&int_part, pre);

	/* Fractional part: (Ref * Frac) / (Pre * 2^24) */
	frac_part = ref_hz * frac;

	frac_part += (pre / 2);
	soc_dp_div64(&frac_part, pre);

	frac_part += (SOC_DP_PLL_FRAC_MOD / 2);
	soc_dp_div64(&frac_part, SOC_DP_PLL_FRAC_MOD);

	vco_hz = int_part + frac_part;

	/* Final Rate = VCO / total_div */
	vco_hz += (total_div / 2); // Rounding before final division
	soc_dp_div64(&vco_hz, total_div);

	vco_hz += 500; // Rounding for 1000
	soc_dp_div64(&vco_hz, 1000);

	return (uint32_t)vco_hz;
}

static int soc_dp_get_pixel_pll_div_total(const struct soc_dp_pixel_pll_cfg *cfg,
					  uint32_t *div_total)
{
	static const uint8_t divm_factors[] = { 1, 2, 3, 5 };

	if (!cfg || !div_total || !cfg->valid || !cfg->divp)
		return -EINVAL;

	if (cfg->div5_en) {
		*div_total = 5;
		return 0;
	}

	if (cfg->divaux == 1) {
		if (cfg->divm >= ARRAY_SIZE(divm_factors))
			return -EINVAL;

		*div_total = 2 * divm_factors[cfg->divm] * cfg->divp;
		return 0;
	}

	if (cfg->divaux < 2)
		return -EINVAL;

	*div_total = 2 * cfg->divaux * cfg->divp;
	return 0;
}

static bool soc_dp_pixel_pll_cfg_matches(const struct soc_dp_pixel_pll_cfg *cfg,
					 uint32_t target_pclk_khz,
					 uint32_t ref_clk_khz)
{
	uint32_t div_total;
	uint32_t actual_pclk_khz;

	if (soc_dp_get_pixel_pll_div_total(cfg, &div_total))
		return false;

	actual_pclk_khz = soc_dp_get_rate_khz(cfg->prediv, cfg->fbdiv, cfg->frac,
					      ref_clk_khz, div_total);

	if (cfg->actual_pclk_khz &&
	    soc_dp_abs_diff(actual_pclk_khz, cfg->actual_pclk_khz) > SOC_DP_PLL_ERR_TOLERANCE)
		return false;

	return soc_dp_abs_diff(actual_pclk_khz, target_pclk_khz) <= SOC_DP_PLL_ERR_TOLERANCE;
}

static const struct soc_dp_pixel_pll_cfg* find_pixel_pll_cfg(uint32_t pclk_khz,
							      uint32_t ref_clk_khz) {
	const struct soc_dp_pixel_pll_cfg *best_match = NULL;
	uint32_t min_diff = 0xFFFFFFFF;
	int num_configs = sizeof(pixel_pll_cfg_table) / sizeof(pixel_pll_cfg_table[0]);

	for (int i = 0; i < num_configs; i++) {
		uint32_t current_target = pixel_pll_cfg_table[i].target_pclk_khz;
		uint32_t diff = (pclk_khz > current_target) ? (pclk_khz - current_target) : (current_target - pclk_khz);

		if (diff == 0 && soc_dp_pixel_pll_cfg_matches(&pixel_pll_cfg_table[i],
							      pclk_khz, ref_clk_khz)) {
			return &pixel_pll_cfg_table[i];
		}

		if ((pclk_khz / 100) == (current_target / 100) &&
		    soc_dp_pixel_pll_cfg_matches(&pixel_pll_cfg_table[i],
						 pclk_khz, ref_clk_khz)) {
			return &pixel_pll_cfg_table[i];
		}

		if (diff < min_diff && diff < 500 &&
		    soc_dp_pixel_pll_cfg_matches(&pixel_pll_cfg_table[i],
						 pclk_khz, ref_clk_khz)) {
			min_diff = diff;
			best_match = &pixel_pll_cfg_table[i];
		}
	}

	return best_match;

}

static int update_edp_config(struct soc_dp_dev *dp, bool enable)
{
	uint8_t value;
	int ret;

	ret = drm_dp_dpcd_read(&dp->aux, DP_EDP_CONFIGURATION_SET, &value, 1);
	if (ret < 0) {
		dev_err(dp->dev, "Failed to read DP_EDP_CONFIGURATION_SET, ret: %d\n", ret);
		return ret;
	}

	if (enable)
		value |= 0x01;
	else
		value &= ~0x01;

	ret = drm_dp_dpcd_write(&dp->aux, DP_EDP_CONFIGURATION_SET, &value, 1);
	if (ret < 0) {
		dev_err(dp->dev, "Failed to write DP_EDP_CONFIGURATION_SET, ret: %d\n", ret);
		return ret;
	}

	return 0;
}

/*
 * Read the Sink's DPCD capability information.
 * Note: EDID is parsed separately. This function focuses solely on
 * Link Layer capabilities (Rate, Lanes, etc.).
 */
static int soc_dp_hw_read_sink_caps(struct soc_dp_dev *dp)
{
	ssize_t ret;
	uint8_t max_bw;
	int retry;

	for (retry = 0; retry < SOC_DP_SINK_READY_RETRIES; retry++) {
		if (retry) {
			soc_dp_aux_hw_reset(dp);
			msleep(SOC_DP_SINK_READY_DELAY_MS);
		}

		/*
		 * drm_dp_read_dpcd_caps() handles DPCD revision <1.3 + 1.3
		 * extended-capability quirks; it returns 0 on success and the
		 * full DP_RECEIVER_CAP_SIZE bytes are filled in.
		 */
		ret = drm_dp_read_dpcd_caps(&dp->aux, dp->dpcd);
		if (ret < 0)
			continue;

		if (!soc_dp_dpcd_caps_valid(dp->dpcd)) {
			dev_dbg(dp->dev,
				"DPCD caps not ready: rev=0x%02x bw=0x%02x lanes=0x%02x\n",
				dp->dpcd[DP_DPCD_REV], dp->dpcd[DP_MAX_LINK_RATE], dp->dpcd[DP_MAX_LANE_COUNT]);
			ret = -EAGAIN;
			continue;
		}

		break;
	}

	if (ret < 0) {
		dev_err(dp->dev, "Failed to read DPCD after %d retries: %zd\n",
			SOC_DP_SINK_READY_RETRIES, ret);
		dp->link.revision = 0x14;
		dp->link.max_rate = SOC_DP_LINK_RATE_5_40;
		dp->link.max_num_lanes = SOC_DP_LANE_2;
		dp->link.enhanced_framing = 1;
		return ret;
	}

	/* 2. Parse DP Revision */
	dp->link.revision = dp->dpcd[DP_DPCD_REV];

	/*
	 * 3. Parse and determine Link Rate.
	 * Get the maximum link rate supported by the Sink.
	 * Note: During link training, we usually start from min(Sink_Max, Source_Max).
	 */
	max_bw = dp->dpcd[DP_MAX_LINK_RATE];
	switch (max_bw) {
	case DP_LINK_BW_1_62:
		dp->link.max_rate = SOC_DP_LINK_RATE_1_62;
		break;
	case DP_LINK_BW_2_7:
		dp->link.max_rate = SOC_DP_LINK_RATE_2_70;
		break;
	case DP_LINK_BW_5_4:
		dp->link.max_rate = SOC_DP_LINK_RATE_5_40;
		break;
	case DP_LINK_BW_8_1:
		dp->link.max_rate = SOC_DP_LINK_RATE_8_10;
		break;
	default:
		dev_warn(dp->dev, "Unknown DPCD Max Rate: 0x%x, defaulting to 5.40G\n", max_bw);
		dp->link.revision = 0x14;
		dp->link.max_rate = SOC_DP_LINK_RATE_5_40;
		dp->link.max_num_lanes = SOC_DP_LANE_2;
		dp->link.enhanced_framing = 1;
		return -1;
	}

	/* 4. Parse and determine Lane Count */
	dp->link.max_num_lanes = dp->dpcd[DP_MAX_LANE_COUNT] & DP_MAX_LANE_COUNT_MASK;

	/* 5. Check for Enhanced Framing support */
	dp->link.enhanced_framing =
		(dp->dpcd[DP_MAX_LANE_COUNT] & DP_ENHANCED_FRAME_CAP);

	dev_info(dp->dev, "DPCD: Rev %x.%x, MaxRate %d kHz, MaxLanes %d, EnhFrame %d\n",
		 dp->link.revision >> 4, dp->link.revision & 0xF,
		 dp->link.max_rate,
		 dp->link.max_num_lanes,
		 dp->link.enhanced_framing);

	return 0;
}

static int soc_dp_is_better_config(bool new_valid, bool new_is_int, uint8_t new_pre, uint32_t new_vco,
				   bool best_valid, bool best_is_int, uint8_t best_pre, uint32_t best_vco)
{
	if (!new_valid)
		return 0;
	if (!best_valid)
		return 1;

	if (new_is_int && !best_is_int)
		return 1;
	if (!new_is_int && best_is_int)
		return 0;

	if (new_pre < best_pre)
		return 1;
	if (new_pre > best_pre)
		return 0;

	if (new_vco > best_vco)
		return 1;

	return 0;
}

static int soc_dp_solve_pll_frac(uint32_t target_vco_khz, uint32_t ref_clk_khz,
				 uint8_t *best_pre, uint16_t *best_fb, uint32_t *best_frac)
{
	uint64_t min_err = ~0ULL;
	int found = 0;
	bool best_is_int = false;
	int pre;

	/* Iterate pre-divider 1 to 63 to find best PFD frequency */
	for (pre = 1; pre <= 63; pre++) {
		uint64_t ref_clk_hz = (uint64_t)ref_clk_khz * 1000;
		uint64_t target_vco_hz = (uint64_t)target_vco_khz * 1000;

		/* Calculate required multiplier: Mult = (TargetVCO * Pre) / Ref */
		uint64_t num = target_vco_hz * pre;
		uint64_t den = ref_clk_hz;
		uint64_t remainder;
		uint64_t fb_val;
		uint64_t frac_val;
		uint64_t actual_vco;
		uint64_t diff;
		bool current_is_int;

		fb_val = num;
		remainder = soc_dp_div64(&fb_val, (uint32_t)den);

		if (fb_val > 4095)
			continue;

		/* Frac = (Remainder * 2^24 + Ref/2) / Ref */
		frac_val = remainder * SOC_DP_PLL_FRAC_MOD;
		frac_val += (den / 2);
		soc_dp_div64(&frac_val, (uint32_t)den);

		if (frac_val > 0xFFFFFF)
			frac_val = 0xFFFFFF;

		/*
		* Calculate actual VCO for error checking.
		* VCO = (Ref * FB / Pre) + (Ref * Frac / (Pre * 2^24))
		*/
		{
			uint64_t vco_int, vco_frac;
			/* Integer part: (Ref * FB) / Pre */
			vco_int = ref_clk_hz * fb_val;
			soc_dp_div64(&vco_int, pre);
			/* Fractional part: (Ref * Frac) / (Pre * 2^24) */
			vco_frac = ref_clk_hz * frac_val;
			soc_dp_div64(&vco_frac, pre);
			soc_dp_div64(&vco_frac, SOC_DP_PLL_FRAC_MOD);

			actual_vco = vco_int + vco_frac;
		}

		diff = soc_dp_abs_diff(actual_vco, target_vco_hz);
		current_is_int = (frac_val == 0);

		if (diff < min_err) {
			min_err = diff;
			*best_pre = pre;
			*best_fb = (uint16_t)fb_val;
			*best_frac = (uint32_t)frac_val;
			best_is_int = current_is_int;
			found = 1;
		} else if (diff == min_err) {
			if (current_is_int && !best_is_int) {
			*best_pre = pre;
			*best_fb = (uint16_t)fb_val;
			*best_frac = (uint32_t)frac_val;
			best_is_int = true;
			found = 1;
			}
		}
	}

	return found ? 0 : -1;
}

static int soc_dp_calc_pixel_pll(uint32_t target_pclk_khz, uint32_t ref_clk_khz, struct soc_dp_pixel_pll_cfg *cfg)
{
	struct soc_dp_pixel_pll_cfg best = {0};
	int pclk_div;

	/* Strategy 1: Div5 Path (VCO = PCLK * 5) */
	{
		struct soc_dp_pixel_pll_cfg curr = {0};
		uint32_t div_total = 5;
		uint32_t target_vco = target_pclk_khz * div_total;

		if (target_vco >= SOC_DP_VCO_MIN_KHZ && target_vco <= SOC_DP_VCO_MAX_KHZ) {
			uint8_t pre;
			uint16_t fb;
			uint32_t frac;

			if (soc_dp_solve_pll_frac(target_vco, ref_clk_khz, &pre, &fb, &frac) == 0) {
			uint32_t actual_pclk = soc_dp_get_rate_khz(pre, fb, frac, ref_clk_khz, div_total);

			if (soc_dp_abs_diff(actual_pclk, target_pclk_khz) <= SOC_DP_PLL_ERR_TOLERANCE) {
				curr.valid = true;
				curr.vco_freq_khz = target_vco;
				curr.actual_pclk_khz = actual_pclk;
				curr.prediv = pre; curr.fbdiv = fb; curr.frac = frac;

				curr.frac_pd = (frac == 0) ? 3 : 0;

				curr.div5_en = 1;
				curr.divaux = 0; curr.divm = 0; curr.divp = 0;

				if (soc_dp_is_better_config(curr.valid, (curr.frac == 0), curr.prediv, curr.vco_freq_khz,
						best.valid, (best.frac == 0), best.prediv, best.vco_freq_khz)) {
					best = curr;
				}
			}
			}
		}
	}

	/* Strategy 2 & 3: Iterate PclkDiv (1 to 31) */
	for (pclk_div = 1; pclk_div <= 31; pclk_div++) {

		/* Strategy 2: DivM Path (DivAux = 1) */
		int divm_factors[] = {1, 2, 3, 5};
		int divm_regs[]    = {0, 1, 2, 3};
		int i;

		for (i = 0; i < 4; i++) {
			struct soc_dp_pixel_pll_cfg curr = {0};
			int m_val = divm_factors[i];
			uint32_t div_total = 2 * m_val * pclk_div;
			uint32_t target_vco = target_pclk_khz * div_total;
			uint8_t pre;
			uint16_t fb;
			uint32_t frac;

			if (target_vco < SOC_DP_VCO_MIN_KHZ || target_vco > SOC_DP_VCO_MAX_KHZ)
				continue;

			if (soc_dp_solve_pll_frac(target_vco, ref_clk_khz, &pre, &fb, &frac) == 0) {
			uint32_t actual_pclk = soc_dp_get_rate_khz(pre, fb, frac, ref_clk_khz, div_total);

			if (soc_dp_abs_diff(actual_pclk, target_pclk_khz) > SOC_DP_PLL_ERR_TOLERANCE)
				continue;

			curr.valid = true;
			curr.vco_freq_khz = target_vco;
			curr.actual_pclk_khz = actual_pclk;
			curr.prediv = pre; curr.fbdiv = fb; curr.frac = frac;

			curr.frac_pd = (frac == 0) ? 3 : 0;

			curr.div5_en = 0;
			curr.divaux = 1;         /* Must be 1 to enable DivM logic */
			curr.divm = divm_regs[i];
			curr.divp = pclk_div;

			if (soc_dp_is_better_config(curr.valid, (curr.frac == 0), curr.prediv, curr.vco_freq_khz,
					best.valid, (best.frac == 0), best.prediv, best.vco_freq_khz)) {
				best = curr;
			}
			}
		}

		/* Strategy 3: DivAux Path (DivAux > 1) */
		{
			int aux;
			for (aux = 2; aux <= 31; aux++) {
			struct soc_dp_pixel_pll_cfg curr = {0};
			uint32_t div_total = 2 * aux * pclk_div;
			uint32_t target_vco = target_pclk_khz * div_total;
			uint8_t pre;
			uint16_t fb;
			uint32_t frac;

			if (target_vco < SOC_DP_VCO_MIN_KHZ || target_vco > SOC_DP_VCO_MAX_KHZ)
				continue;

			if (soc_dp_solve_pll_frac(target_vco, ref_clk_khz, &pre, &fb, &frac) == 0) {
				uint32_t actual_pclk = soc_dp_get_rate_khz(pre, fb, frac, ref_clk_khz, div_total);

				if (soc_dp_abs_diff(actual_pclk, target_pclk_khz) > SOC_DP_PLL_ERR_TOLERANCE)
					continue;

				curr.valid = true;
				curr.vco_freq_khz = target_vco;
				curr.actual_pclk_khz = actual_pclk;
				curr.prediv = pre; curr.fbdiv = fb; curr.frac = frac;

				curr.frac_pd = (frac == 0) ? 3 : 0;

				curr.div5_en = 0;
				curr.divaux = aux;
				curr.divm = 0; /* Ignored when divaux != 1 */
				curr.divp = pclk_div;

				if (soc_dp_is_better_config(curr.valid, (curr.frac == 0), curr.prediv, curr.vco_freq_khz,
						best.valid, (best.frac == 0), best.prediv, best.vco_freq_khz)) {
				best = curr;
				}
			}
			}
		}
	}

	if (!best.valid)
		return -EINVAL;

	*cfg = best;
	return 0;
}

static void soc_dp_calc_core_pll_to_reg(struct soc_dp_dev *dp, struct soc_dp_core_pll_cfg *cfg)
{
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_PD, 1);
	mdelay(2);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_PREDIV, cfg->prediv);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_FBDIV_LBIT, cfg->fbdiv & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_FBDIV_HBIT, (cfg->fbdiv >> 8) & 0xF);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_DACPD, (cfg->frac_pd >> 1) & 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_DSMPD, cfg->frac_pd & 0x1);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_FRAC_LBIT, cfg->frac & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_FRAC_MBIT, (cfg->frac >> 8) & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_FRAC_HBIT, (cfg->frac >> 16) & 0xFF);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_POSTDIV, cfg->postdiv_reg);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_POSTDIVEN, cfg->postdiv_en);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_VCOCLK_DIV8_EN, cfg->vcoclk_div8_en);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_CLKDIV_16M, cfg->clk_16mdiv);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_LOCK_BYPEN, 1);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_PD, 0);
	mdelay(2);
}

static void soc_dp_calc_pixel_pll_to_reg(struct soc_dp_dev *dp, const struct soc_dp_pixel_pll_cfg *cfg)
{
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PD, 1);
	mdelay(2);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PREDIV, cfg->prediv);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_FBDIV2_LBIT, cfg->fbdiv & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_FBDIV2_HBIT, (cfg->fbdiv >> 8) & 0xF);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_DACPD, (cfg->frac_pd >> 1) & 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_DSMPD, cfg->frac_pd & 0x1);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_FRAC2_LBIT, cfg->frac & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_FRAC2_MBIT, (cfg->frac >> 8) & 0xFF);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_FRAC2_HBIT, (cfg->frac >> 16) & 0xFF);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PRECLK_DIVM, cfg->divm);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PRECLK_DIVAUX, cfg->divaux);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PCLKDIV5_EN, cfg->div5_en);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PCLK_DIVAUX, cfg->divp);

	soc_dp_reg_write_range(dp, SOC_DPTX_REG_PCLK_OUTPUT_NORMAL, 1);
	mdelay(2);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PD, 0);
	mdelay(2);
}

static int soc_dp_hw_set_pll(struct soc_dp_dev *dp, enum soc_dp_link_rate rate, uint32_t pclk)
{
	const struct soc_dp_pixel_pll_cfg *pixel_pll_cfg;
	int ret;

	dev_info(dp->dev, "Setting PLL to Rate %d kHz, Pclk %d kHz\n", rate, pclk);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_DP_EN, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_HDMI_EN, 0);

	if (rate == SOC_DP_LINK_RATE_1_62) {
		soc_dp_calc_core_pll_to_reg(dp, &core_pll_cfg_table[0]);
	} else if (rate == SOC_DP_LINK_RATE_2_70 ) {
		soc_dp_calc_core_pll_to_reg(dp, &core_pll_cfg_table[1]);
	} else if (rate == SOC_DP_LINK_RATE_5_40 ) {
		soc_dp_calc_core_pll_to_reg(dp, &core_pll_cfg_table[2]);
	} else {
		dev_err(dp->dev, "Unsupported link rate %d\n", rate);
		return -EINVAL;
	}

	pixel_pll_cfg = find_pixel_pll_cfg(pclk, dp->ref_clk);
	if (pixel_pll_cfg) {
		soc_dp_calc_pixel_pll_to_reg(dp, pixel_pll_cfg);
	} else {
		struct soc_dp_pixel_pll_cfg pll_cfg;

		memset(&pll_cfg, 0, sizeof(pll_cfg));
		ret = soc_dp_calc_pixel_pll(pclk, dp->ref_clk, &pll_cfg);
		if (ret) {
			dev_err(dp->dev, "Unsupported pixel clock %d\n", pclk);
			return ret;
		}
		dev_info(dp->dev, "pixel pll config: prediv %d, fbdiv %d, frac_pd %d, frac %d, div5_en %d, divm %d, divaux %d, divp %d\n",
			 pll_cfg.prediv, pll_cfg.fbdiv, pll_cfg.frac_pd, pll_cfg.frac, pll_cfg.div5_en, pll_cfg.divm, pll_cfg.divaux, pll_cfg.divp);
		soc_dp_calc_pixel_pll_to_reg(dp, &pll_cfg);
	}

	return 0;
}

static int soc_dp_phy_set_lanes(struct soc_dp_dev *dp,
				enum soc_dp_lane_count lanes)
{
	union phy_configure_opts opts = {
		.dp = {
			.lanes = lanes,
			.set_lanes = true,
		},
	};
	int ret;

	ret = phy_configure(dp->phy, &opts);
	if (ret)
		return ret;

	dp->lane_count = lanes;
	return 0;
}

static int soc_dp_phy_set_rate(struct soc_dp_dev *dp,
			       enum soc_dp_link_rate rate)
{
	union phy_configure_opts opts = {
		.dp = {
			/* enum value is in kbit/s; PHY framework wants Mbit/s. */
			.link_rate = rate / 1000,
			.set_rate = true,
		},
	};
	int ret;

	ret = phy_configure(dp->phy, &opts);
	if (ret)
		return ret;

	dp->link_rate = rate;
	return 0;
}

/*
 * phy_power_on() / phy_power_off() in the PHY framework are refcounted
 * and only invoke ops->power_on / ops->power_off on a 0->1 or 1->0
 * edge. The controller's bring-up logic is idempotent ("ensure on" /
 * "ensure off") rather than pair-balanced, so wrap the framework calls
 * with a controller-side bool. Without this wrapper, calling
 * phy_power_off twice (once after probe and once at the top of the
 * first atomic_enable retry) drives the count negative and the next
 * phy_power_on silently skips ops->power_on, leaving the MPLL /
 * PREPLL / XMIT_ENABLE bits in their last state -- CR then fails on
 * every link config because the transmitter is not actually running.
 */
static int soc_dp_phy_on(struct soc_dp_dev *dp)
{
	int ret;

	if (dp->phy_powered)
		return 0;

	ret = phy_power_on(dp->phy);
	if (ret)
		return ret;

	dp->phy_powered = true;
	return 0;
}

static void soc_dp_phy_off(struct soc_dp_dev *dp)
{
	if (!dp->phy_powered)
		return;

	phy_power_off(dp->phy);
	dp->phy_powered = false;
}

/*
 * Tear the DP link down: stop the video stream, power the PHY off,
 * then pulse the AUX block reset so the next bring-up sees a clean
 * AUX state machine. The pulse sat inside the old soc_dp_phy_power_off
 * helper; pulling it back out into the controller keeps the PHY driver
 * from having to write the controller's AUX_RESET register.
 */
static void soc_dp_link_disable(struct soc_dp_dev *dp)
{
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_STREAM_ENABLE, 0);
	soc_dp_phy_off(dp);

	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x1);
	mdelay(5);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x0);
	mdelay(2);
}

/*
 * Check Hot Plug Detect (HPD) Status
 */
static enum drm_connector_status soc_dp_hw_detect_hpd(struct soc_dp_dev *dp)
{
	uint32_t hpd_status;
	enum drm_connector_status connector_status = connector_status_disconnected;

	soc_dp_reg_read_range(dp, SOC_DPTX_HPD_IN_STATUS, &hpd_status);
	if (hpd_status)
		connector_status = connector_status_connected;
	else
		connector_status = connector_status_disconnected;

	return connector_status;
}

/*
 * Clean Hot Plug Detect (HPD) Status
 */
static void soc_dp_hw_clean_hpd(struct soc_dp_dev *dp)
{
	uint32_t plug_event, unplug_event;

	soc_dp_reg_read_range(dp, SOC_DPTX_HOT_PLUG_EVENT, &plug_event);
	soc_dp_reg_read_range(dp, SOC_DPTX_HOT_UNPLUG_EVENT, &unplug_event);

	if (plug_event)
		soc_dp_reg_only_write_range(dp, SOC_DPTX_HOT_PLUG_EVENT, 0x1);

	if (unplug_event)
		soc_dp_reg_only_write_range(dp, SOC_DPTX_HOT_UNPLUG_EVENT, 0x1);
}

static int soc_dp_set_training_pattern(struct soc_dp_dev *dp, uint8_t pattern)
{
	uint32_t tps_sel = 0;
	uint8_t dpcd_pattern = pattern;
	int ret;

	if (pattern != DP_TRAINING_PATTERN_DISABLE)
		dpcd_pattern |= DP_LINK_SCRAMBLING_DISABLE;

	/* Configure PHY Pattern */
	switch (pattern) {
	case DP_TRAINING_PATTERN_DISABLE:
		tps_sel = 0;
		soc_dp_reg_write_range(dp, SOC_DPTX_SCRAMBLER_DISABLE, 0);
		break;
	case DP_TRAINING_PATTERN_1:
		tps_sel = 1;
		soc_dp_reg_write_range(dp, SOC_DPTX_SCRAMBLER_DISABLE, 1);
		break;
	case DP_TRAINING_PATTERN_2:
		tps_sel = 2;
		soc_dp_reg_write_range(dp, SOC_DPTX_SCRAMBLER_DISABLE, 1);
		break;
	case DP_TRAINING_PATTERN_3:
		tps_sel = 3;
		soc_dp_reg_write_range(dp, SOC_DPTX_SCRAMBLER_DISABLE, 1);
		break;
	default:
		dev_err(dp->dev, "Unsupported training pattern: 0x%x\n", pattern);
		return -EINVAL;
	}

	soc_dp_reg_write_range(dp, SOC_DPTX_TPS_SEL, tps_sel);

	/* Configure DPCD Pattern */
	ret = drm_dp_dpcd_writeb(&dp->aux, DP_TRAINING_PATTERN_SET, dpcd_pattern);
	if (ret < 0) {
		dev_err(dp->dev, "Failed to set DPCD training pattern: %d\n", ret);
		return ret;
	}

	return 0;
}

/*
 * Apply the sink's per-lane adjust request: program the source PHY's
 * vswing / pre-emphasis levels through phy_configure() and reflect the
 * same levels back into the sink's DPCD DP_TRAINING_LANE*_SET. Before
 * this helper was split out, the loop only wrote DPCD; the source PHY
 * stayed at its reset default no matter what the sink asked for, so
 * any link that required level > 0 trained with a degraded eye.
 */
static int soc_dp_link_apply_adjust(struct soc_dp_dev *dp,
				    const u8 *link_status,
				    enum soc_dp_lane_count lanes,
				    u8 *training_set)
{
	union phy_configure_opts opts = {
		.dp = {
			.lanes = lanes,
			.set_voltages = true,
		},
	};
	int i, ret;

	for (i = 0; i < lanes; i++) {
		u8 v = drm_dp_get_adjust_request_voltage(link_status, i);
		u8 p = drm_dp_get_adjust_request_pre_emphasis(link_status, i);
		u8 v_bits;

		if (v >= SOC_DP_SWING_MAX)
			v = SOC_DP_SWING_MAX;
		if (p >= SOC_DP_PREEMP_MAX)
			p = SOC_DP_PREEMP_MAX;

		opts.dp.voltage[i] = v;
		opts.dp.pre[i] = p;

		v_bits = v;
		if (v == SOC_DP_SWING_MAX)
			v_bits |= DP_TRAIN_MAX_SWING_REACHED;
		if (p == SOC_DP_PREEMP_MAX)
			v_bits |= DP_TRAIN_MAX_PRE_EMPHASIS_REACHED;

		training_set[i] = v_bits | (p << DP_TRAIN_PRE_EMPHASIS_SHIFT);
	}

	ret = phy_configure(dp->phy, &opts);
	if (ret)
		return ret;

	ret = drm_dp_dpcd_write(&dp->aux, DP_TRAINING_LANE0_SET,
				training_set, lanes);
	return ret < 0 ? ret : 0;
}

static int soc_dp_link_train_clock_recovery(struct soc_dp_dev *dp, enum soc_dp_link_rate rate, enum soc_dp_lane_count lanes)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	uint8_t training_set[4] = {0};
	int retries = 0;
	int ret;

	ret = drm_dp_dpcd_write(&dp->aux, DP_TRAINING_LANE0_SET,
				training_set, lanes);
	if (ret < 0)
		return ret;

	ret = soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_1);
	if (ret < 0) {
		soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
		return ret;
	}

	while (retries < 8) {
		drm_dp_link_train_clock_recovery_delay(&dp->aux, dp->dpcd);

		ret = drm_dp_dpcd_read_link_status(&dp->aux, link_status);
		if (ret < 0) {
			soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
			return ret;
		}

		if (drm_dp_clock_recovery_ok(link_status, lanes))
			return 0;

		ret = soc_dp_link_apply_adjust(dp, link_status, lanes,
					       training_set);
		if (ret) {
			soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
			return ret;
		}

		retries++;
	}

	dev_err(dp->dev, "Link Training Clock Recovery Failed\n");
	soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
	return -ETIMEDOUT;
}

static int soc_dp_link_train_channel_eq(struct soc_dp_dev *dp, enum soc_dp_link_rate rate, enum soc_dp_lane_count lanes)
{
	uint8_t link_status[DP_LINK_STATUS_SIZE];
	uint8_t training_set[4] = {0};
	uint8_t training_pattern = DP_TRAINING_PATTERN_2;
	int retries = 0;
	int ret;

	if (dp->dpcd[DP_MAX_LANE_COUNT] & DP_TPS3_SUPPORTED) {
		training_pattern = DP_TRAINING_PATTERN_3;
		dev_info(dp->dev, "Link Training: Using TPS3\n");
	} else {
		dev_info(dp->dev, "Link Training: Using TPS2\n");
	}

	ret = soc_dp_set_training_pattern(dp, training_pattern);
	if (ret < 0) {
		soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
		return ret;
	}

	while (retries < 8) {
		drm_dp_link_train_channel_eq_delay(&dp->aux, dp->dpcd);

		ret = drm_dp_dpcd_read_link_status(&dp->aux, link_status);
		if (ret < 0) {
			soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
			return ret;
		}

		if (drm_dp_channel_eq_ok(link_status, lanes)) {
			soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
			return 0;
		}

		ret = soc_dp_link_apply_adjust(dp, link_status, lanes,
					       training_set);
		if (ret) {
			soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
			return ret;
		}

		retries++;
	}

	dev_err(dp->dev, "Link Training Channel EQ Failed\n");
	soc_dp_set_training_pattern(dp, DP_TRAINING_PATTERN_DISABLE);
	return -ETIMEDOUT;
}

/*
 * Main Link Training Function
 */
static int soc_dp_link_train(struct soc_dp_dev *dp, enum soc_dp_link_rate rate, enum soc_dp_lane_count lanes)
{
	int ret;
	uint8_t link_config[2];
	uint8_t bw_code;

	/* Map Link Rate Enum to DPCD Bandwidth Code */
	switch (rate) {
	case SOC_DP_LINK_RATE_1_62:
		bw_code = DP_LINK_BW_1_62;
		break;
	case SOC_DP_LINK_RATE_2_70:
		bw_code = DP_LINK_BW_2_7;
		break;
	case SOC_DP_LINK_RATE_5_40:
		bw_code = DP_LINK_BW_5_4;
		break;
	case SOC_DP_LINK_RATE_8_10:
		bw_code = DP_LINK_BW_8_1;
		break;
	default:
		bw_code = DP_LINK_BW_1_62;
		break;
	}

	/* Configure DPCD Link Rate and Lane Count */
	link_config[0] = bw_code;
	link_config[1] = lanes;
	if (dp->link.enhanced_framing)
		link_config[1] |= DP_LANE_COUNT_ENHANCED_FRAME_EN;

	ret = drm_dp_dpcd_write(&dp->aux, DP_LINK_BW_SET, link_config, 2);
	if (ret < 0) {
		dev_err(dp->dev, "Failed to configure DPCD\n");
		return ret;
	}

	ret = soc_dp_link_train_clock_recovery(dp, rate, lanes);
	if (ret)
		return ret;

	ret = soc_dp_link_train_channel_eq(dp, rate, lanes);
	if (ret)
		return ret;

	return 0;
}

static void soc_dp_hw_set_msa_and_enable_video(struct soc_dp_dev *dp, const struct drm_display_mode *mode,
		enum soc_dp_link_rate rate, enum soc_dp_lane_count lanes,
		uint32_t color_format)
{
	uint64_t hb_num;
	uint32_t link_rate;
	uint32_t fp; // Pixel clock in MHz
	uint32_t bpp, misc0;
	uint32_t tu, tu_frac, tu_int, rd_thres;
	uint32_t hsync_len;

	// 1. Prepare basic parameters
	// mode->clock unit is kHz, fp unit is MHz
	if (dp->use_ext_pixel_clock)
		fp = dp->pixel_clock / 1000;
	else
		fp = mode->clock / 1000;

	if (fp == 0)
		fp = 1;  // Prevent division by zero

	// Get BPP
	bpp = soc_dp_get_bpp(color_format);

	// Calculate MISC0
	// bit0: 0 (Sync Clock)
	// bits1-7: Color Format (000=RGB, 001=YCbCr422, 010=YCbCr444)
	// bits5-7: BPC (001=8bpc, 010=10bpc, etc)
	switch (color_format) {
	case SOC_VIDEO_RGB_6BIT:
		misc0 = 0x00;
		break;
	case SOC_VIDEO_RGB_8BIT:
		misc0 = 0x20;
		break;
	case SOC_VIDEO_RGB_10BIT:
		misc0 = 0x40;
		break;
	case SOC_VIDEO_RGB_12BIT:
		misc0 = 0x60;
		break;
	case SOC_VIDEO_RGB_16BIT:
		misc0 = 0x80;
		break;
	case SOC_VIDEO_YUV422_8BIT:
		misc0 = 0x22;
		break;
	case SOC_VIDEO_YUV422_10BIT:
		misc0 = 0x42;
		break;
	case SOC_VIDEO_YUV422_12BIT:
		misc0 = 0x62;
		break;
	case SOC_VIDEO_YUV422_16BIT:
		misc0 = 0x82;
		break;
	case SOC_VIDEO_YUV444_8BIT:
		misc0 = 0x24;
		break;
	case SOC_VIDEO_YUV444_10BIT:
		misc0 = 0x44;
		break;
	case SOC_VIDEO_YUV444_12BIT:
		misc0 = 0x64;
		break;
	case SOC_VIDEO_YUV444_16BIT:
		misc0 = 0x84;
		break;
	default:
		misc0 = 0x20;
		break;
	}

	// 2. Calculate HBlank Interval (hb_num)
	// (htotal - hactive) * (LinkSymbolClock / 4) / PixelClock
	// LinkSymbolClock = LinkRate * 100 (e.g., 1.62G -> 162MHz)
	// rate unit is kHz (e.g., 1620000)
	// link_rate = rate / 10000 (e.g., 162)
	link_rate = rate / 10000;

	// Formula: hb_num = hblank * (link_rate / 4) / fp
	// To avoid floating point arithmetic, multiply first then divide
	hb_num = (uint64_t)(mode->htotal - mode->hdisplay) * link_rate;
	do_div(hb_num, 4 * fp);

	// 3. Calculate TU (Transfer Unit)
	// tu = fp * bpp * 640 / (8 * num_cnt * link_rate)
	// Here link_rate also refers to 162, 270 etc.
	{
		uint64_t temp_tu = (uint64_t)fp * bpp * 640;
		uint32_t den = 8 * lanes * link_rate;
		do_div(temp_tu, den);
		tu = temp_tu;
	}
	tu_frac = tu % 10;
	tu_int  = tu / 10;

	// 4. Calculate FIFO read threshold
	if (tu_int < 6) {
		rd_thres = 32;
	} else if ((mode->htotal - mode->hdisplay) < 80) {
		rd_thres = 12;
	} else {
		rd_thres = 16;
	}

	dev_info(dp->dev, "MSA: %dx%d, Rate:%d kHz, Lanes:%d, BPP:%d, TU:%d.%d\n",
		 mode->hdisplay, mode->vdisplay, rate, lanes, bpp, tu_int, tu_frac);

	// 5. Video mapping format
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_MAPPING, color_format);

	// Polarity configuration
	if (mode->flags & DRM_MODE_FLAG_PHSYNC)
		soc_dp_reg_write_range(dp, SOC_DPTX_HSYNC_IN_POLARITY, 1);
	else
		soc_dp_reg_write_range(dp, SOC_DPTX_HSYNC_IN_POLARITY, 0);

	if (mode->flags & DRM_MODE_FLAG_PVSYNC)
		soc_dp_reg_write_range(dp, SOC_DPTX_VSYNC_IN_POLARITY, 1);
	else
		soc_dp_reg_write_range(dp, SOC_DPTX_VSYNC_IN_POLARITY, 0);

	soc_dp_reg_write_range(dp, SOC_DPTX_HSYNC_IN_POLARITY, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_VSYNC_IN_POLARITY, 1);

	// Basic timing
	soc_dp_reg_write_range(dp, SOC_DPTX_HACTIVE, mode->hdisplay);
	soc_dp_reg_write_range(dp, SOC_DPTX_VACTIVE, mode->vdisplay);
	soc_dp_reg_write_range(dp, SOC_DPTX_HBLANK, mode->htotal - mode->hdisplay);
	soc_dp_reg_write_range(dp, SOC_DPTX_VBLANK, mode->vtotal - mode->vdisplay);

	soc_dp_reg_write_range(dp, SOC_DPTX_HSTART, mode->htotal - mode->hsync_end + (mode->hsync_end - mode->hsync_start));
	soc_dp_reg_write_range(dp, SOC_DPTX_VSTART, mode->vtotal - mode->vsync_end + (mode->vsync_end - mode->vsync_start));

	hsync_len = mode->hsync_end - mode->hsync_start;

	soc_dp_reg_write_range(dp, SOC_DPTX_H_SYNC_WIDTH, hsync_len);
	soc_dp_reg_write_range(dp, SOC_DPTX_V_SYNC_WIDTH, mode->vsync_end - mode->vsync_start);
	soc_dp_reg_write_range(dp, SOC_DPTX_H_FRONT_PORCH, mode->hsync_start - mode->hdisplay);
	soc_dp_reg_write_range(dp, SOC_DPTX_V_FRONT_PORCH, mode->vsync_start - mode->vdisplay);

	// MSA and MISC
	soc_dp_reg_write_range(dp, SOC_DPTX_MISC0, misc0);
	soc_dp_reg_write_range(dp, SOC_DPTX_MISC1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_NVID, 0);

	// Link layer parameters
	soc_dp_reg_write_range(dp, SOC_DPTX_HBLANK_INTERVAL, (uint32_t)hb_num);
	soc_dp_reg_write_range(dp, SOC_DPTX_AVERAGE_BYTES_PER_TU, tu_int);
	soc_dp_reg_write_range(dp, SOC_DPTX_AVERAGE_BYTES_PER_TU_FRAC, tu_frac);
	soc_dp_reg_write_range(dp, SOC_DPTX_INIT_THRESHOLD, rd_thres);

	soc_dp_reg_write_range(dp, SOC_DPTX_PHY_SSC_DIS, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_REG_VID_CLK_SEL, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VID_BIST_EN, 0);

	// 6. Enable video stream
	dev_dbg(dp->dev, "Enabling Video Stream...\n");
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_STREAM_ENABLE, 1);
}

static void soc_dp_hw_disable(struct soc_dp_dev *dp)
{
	dev_info(dp->dev, "Disabling Video & PHY\n");

	/* 1. Disable Video Stream */
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_STREAM_ENABLE, 0);

	/* 2. Disable Transmitters */
	soc_dp_reg_write_range(dp, SOC_DPTX_XMIT_ENABLE, 0);

	/* Disable LDO */
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_LDO_D0, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_LDO_D1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_LDO_D2, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_LDO_D3, 0);

	/* Disable Driver */
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_DRV_D0, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_DRV_D1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_DRV_D2, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_DRV_D3, 0);

	/* Disable P2S */
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_P2S_D0, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_P2S_D1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_P2S_D2, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_EN_P2S_D3, 0);

	/*Disable BG_EN */
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_BG_EN, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_BG_EN_RCAL, 0);

	/* 3. Power Down PLLs (MPLL and PREPLL) */
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_PD, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PD, 1);
}

/* Calculate required bandwidth in kbps (Pixel Clock * Bits Per Pixel) */
static uint32_t soc_dp_calc_required_bw(const struct drm_display_mode *mode, int bpp)
{
	return mode->clock * bpp;
}

/* Calculate available link capacity in kbps (taking 8b/10b overhead into account) */
static uint32_t soc_dp_calc_link_capacity(enum soc_dp_link_rate rate, enum soc_dp_lane_count lanes)
{
	/* Capacity = Rate(kHz) * Lanes * 0.8 */
	return (rate * lanes * 8) / 10;
}

static enum drm_connector_status
soc_dp_bridge_detect(struct drm_bridge *bridge, struct drm_connector *connector)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);
	enum drm_connector_status status;

	/*
	 * eDP is treated as "panel-edp owns the connection": before
	 * soc_dp_edp_link_panel() (the AUX-bus done_probing callback)
	 * has chained panel-edp into the bridge chain we report
	 * DISCONNECTED so drm_fb_helper_initial_config() does not
	 * commit a wedged modeset against a chain that still lacks
	 * panel-edp; afterwards we report CONNECTED unconditionally
	 * because the eDP panel is a fixed part of the board (no
	 * hot-plug). The controller's SOC_DPTX_HPD_IN_STATUS register
	 * does not read meaningfully on the eDP path -- the line is
	 * panel-driven, not cable-driven -- so dp->connector_status
	 * (which the IRQ handler derives from that register) is
	 * intentionally ignored for the eDP detect path. This matches
	 * the upstream convention used by ti-sn65dsi86 / analogix_dp
	 * and friends.
	 *
	 * Boards with truly hot-pluggable eDP (rare) would need a
	 * follow-up patch that delegates HPD to panel-edp's
	 * drm_panel_bridge ops; out of scope for v1. The eDP IRQ
	 * itself stays wired (the controller IP shares the same line
	 * with potential link-error / SDP / AUX-fail diagnostics that
	 * a future change may want to surface).
	 */
	if (dp->edp_mode) {
		if (!dp->next_bridge)
			return connector_status_disconnected;
		return connector_status_connected;
	}

	/*
	 * DP is cable-driven, but the controller's HPD interrupt is gated
	 * by the DP pixel clock (off while idle/disconnected), so it can
	 * never report a plug. Read the HPD level register live on every
	 * detect instead; drm_kms_helper_poll drives this periodically
	 * because the connector is marked POLL_CONNECT | POLL_DISCONNECT
	 * (the DP bridge does not advertise DRM_BRIDGE_OP_HPD).
	 */
	mutex_lock(&dp->mode_lock);
	status = soc_dp_hw_detect_hpd(dp);
	dp->connector_status = status;
	mutex_unlock(&dp->mode_lock);

	return status;
}

static int soc_dp_aux_i2c_write(struct soc_dp_dev *dp, u32 address,
				const u8 *buf, size_t len)
{
	struct drm_dp_aux_msg msg = {
		.address = address,
		.request = DP_AUX_I2C_WRITE,
		.buffer = (u8 *)buf,
		.size = len,
	};
	int ret;

	ret = soc_dp_aux_transfer(&dp->aux, &msg);
	if (ret < 0)
		return ret;

	return ret == len ? 0 : -EIO;
}

static int soc_dp_aux_i2c_read(struct soc_dp_dev *dp, u32 address,
			       u8 *buf, size_t len)
{
	struct drm_dp_aux_msg msg = {
		.address = address,
		.request = DP_AUX_I2C_READ,
		.buffer = buf,
		.size = len,
	};
	int ret;

	ret = soc_dp_aux_transfer(&dp->aux, &msg);
	if (ret < 0)
		return ret;

	return ret == len ? 0 : -EIO;
}

static int soc_dp_conn_get_edid_block(void *data, u8 *buf,
				      unsigned int block, size_t len)
{
	struct soc_dp_dev *dp = data;
	unsigned int start = block * EDID_LENGTH;
	u8 segment = block >> 1;
	int ret, retry;
	size_t offset;

	if (segment) {
		for (retry = 0; retry < 3; retry++) {
			ret = soc_dp_aux_i2c_write(dp, SOC_DP_DDC_SEGMENT_ADDR,
						   &segment, 1);
			if (!ret)
				break;
		}

		if (ret) {
			dev_err(dp->dev,
				"[EDID] segment write failed, block %u segment %u ret %d\n",
				block, segment, ret);
			return ret;
		}
	}

	for (offset = 0; offset < len; offset += SOC_DP_EDID_CHUNK_SIZE) {
		u8 edid_offset = (start + offset) & 0xff;
		size_t chunk = min_t(size_t, SOC_DP_EDID_CHUNK_SIZE, len - offset);

		for (retry = 0; retry < 3; retry++) {
			ret = soc_dp_aux_i2c_write(dp, DDC_ADDR, &edid_offset, 1);
			if (ret)
				continue;

			ret = soc_dp_aux_i2c_read(dp, DDC_ADDR, buf + offset, chunk);
			if (!ret)
				break;
		}

		if (ret) {
			dev_err(dp->dev,
				"[EDID] read failed, block %u offset 0x%02x len %zu ret %d\n",
				block, edid_offset, chunk, ret);
			return ret;
		}
	}

	return 0;
}

static const struct drm_edid *
soc_dp_bridge_edid_read(struct drm_bridge *bridge, struct drm_connector *connector)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);
	const struct drm_edid *edid = NULL;
	int retry;

	mutex_lock(&dp->mode_lock);

	for (retry = 0; retry < SOC_DP_SINK_READY_RETRIES; retry++) {
		if (retry) {
			soc_dp_aux_hw_reset(dp);
			msleep(SOC_DP_SINK_READY_DELAY_MS);
		}

		edid = drm_edid_read_custom(connector, soc_dp_conn_get_edid_block, dp);
		if (edid)
			break;
	}

	mutex_unlock(&dp->mode_lock);

	if (!edid)
		dev_err(dp->dev, "Failed to read EDID\n");

	return edid;
}

static enum drm_mode_status
soc_dp_bridge_mode_valid(struct drm_bridge *bridge,
			 const struct drm_display_info *info,
			 const struct drm_display_mode *mode)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);
	unsigned int max_link_rate, max_lanes;
	u64 max_bw, req_bw;
	const unsigned int bpp = 24;

	max_link_rate = drm_dp_max_link_rate(dp->dpcd);
	max_lanes = drm_dp_max_lane_count(dp->dpcd);
	if (!max_link_rate || !max_lanes) {
		/* Sink not enumerated yet -- accept and re-check at modeset. */
		return MODE_OK;
	}

	/*
	 * drm_dp_max_link_rate() returns the per-lane rate in units of
	 * 10 kHz (the DP spec's bw_code = rate / 0.27 Gbps encoding),
	 * not in kHz: HBR2 reports as 540000 (= 5.4 Gbps), not 5400000.
	 * Multiply by 10 to get kHz before computing total link bandwidth
	 * in kbit/s; then apply the 8b/10b coding overhead. The 10x and
	 * 1/10 cancel and we are left with "* 8".
	 */
	max_bw = (u64)max_link_rate * max_lanes * 8;

	/* Required bandwidth: pixel clock (kHz) * bits per pixel. */
	req_bw = (u64)mode->clock * bpp;

	if (req_bw > max_bw) {
		drm_dbg_kms(bridge->dev,
			    "rejecting %dx%d@%d (%llu kbit/s) > sink cap (%llu kbit/s; %u kHz x %u lanes)\n",
			    mode->hdisplay, mode->vdisplay, drm_mode_vrefresh(mode),
			    req_bw, max_bw, max_link_rate * 10, max_lanes);
		return MODE_CLOCK_HIGH;
	}

	return MODE_OK;
}

static const struct drm_encoder_funcs soc_dp_encoder_funcs = {
	.destroy = drm_encoder_cleanup,
};

static void soc_dp_bridge_atomic_enable(struct drm_bridge *bridge,
					struct drm_atomic_commit *state)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);
	struct drm_bridge_state *new_bridge_state =
		drm_atomic_get_new_bridge_state(state, bridge);
	struct soc_dp_bridge_state *st = to_soc_dp_bridge_state(new_bridge_state);
	struct drm_connector_state *conn_state =
		drm_atomic_get_new_connector_state(state, dp->connector);
	struct drm_crtc_state *crtc_state =
		drm_atomic_get_new_crtc_state(state, conn_state->crtc);
	struct drm_display_mode *adjusted_mode = &crtc_state->adjusted_mode;
	const struct soc_dp_link_config *cfg;
	uint64_t clk_val;
	uint64_t set_clk_val;
	u32 req_bw;
	int start;
	int bpp;
	int i;

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);
	drm_mode_copy(&dp->mode, adjusted_mode);

	if (dp->use_ext_pixel_clock && dp->pxclk) {
		set_clk_val = adjusted_mode->clock * 1000;
		if (set_clk_val) {
			set_clk_val = clk_round_rate(dp->pxclk, set_clk_val);
			clk_val = clk_get_rate(dp->pxclk);
			if(clk_val != set_clk_val){
				clk_set_rate(dp->pxclk, set_clk_val);
				DRM_INFO("set dp pxclk=%lld\n", set_clk_val);
			}
		}
		clk_val = clk_get_rate(dp->pxclk);
		dp->pixel_clock = clk_val / 1000;
	}

	bpp = soc_dp_get_bpp(st->color_format);
	req_bw = soc_dp_calc_required_bw(adjusted_mode, bpp);

	/*
	 * ->atomic_check() picked the first table entry that satisfies both
	 * the source/sink caps and @req_bw. Start training from there, but
	 * keep the upgrade walk in case link training itself fails on that
	 * candidate. If atomic_check did not run (idx still -1), fall back
	 * to walking the whole table.
	 */
	start = (st && st->link_cfg_idx >= 0) ? st->link_cfg_idx : 0;

	for (i = start; i < ARRAY_SIZE(soc_dp_link_priority_table); i++) {
		uint32_t capacity;

		cfg = &soc_dp_link_priority_table[i];

		/* Filter 1: Check HW Capabilities (Source & Sink limits) */
		if (cfg->rate > dp->link.max_rate || cfg->lanes > dp->link.max_num_lanes)
			continue;

		/* Filter 2: Check Bandwidth Requirement */
		capacity = soc_dp_calc_link_capacity(cfg->rate, cfg->lanes);
		if (capacity < req_bw)
			continue;

		dev_info(dp->dev, "DP: Attempting Config: R=%d, L=%d (Cap: %d > Req: %d)\n",
			cfg->rate, cfg->lanes, capacity, req_bw);

		soc_dp_link_disable(dp);

		/* Apply Hardware Settings */
		if (dp->use_ext_pixel_clock) {
			if (soc_dp_hw_set_pll(dp, cfg->rate, dp->pixel_clock))
				continue;
		} else {
			if (soc_dp_hw_set_pll(dp, cfg->rate, adjusted_mode->clock))
				continue;
		}

		if (soc_dp_phy_set_lanes(dp, cfg->lanes))
			continue;
		if (soc_dp_phy_set_rate(dp, cfg->rate))
			continue;

		if (soc_dp_phy_on(dp))
			continue;

		/*
		 * eDP HPD often bounces during training on real hardware (the
		 * panel is hardwired, the HPD line is electrically noisy).
		 * Only honour HPD as a training gate for DP, not eDP.
		 */
		if (!dp->edp_mode &&
		    soc_dp_hw_detect_hpd(dp) == connector_status_disconnected) {
			mutex_unlock(&dp->mode_lock);
			dev_warn(dp->dev, "DP: Training failed for the connector is disconnected\n");
			return;
		}

		if (dp->edp_mode) {
			soc_dp_reg_write_range(dp, SOC_DPTX_ENABLE_EDP, 0x1);
			soc_dp_reg_write_range(dp, SOC_DPTX_STREAM_ENC_EN, 0x1);
			update_edp_config(dp, true);
		}

		/* Execute Link Training */
		if (soc_dp_link_train(dp, cfg->rate, cfg->lanes) == 0) {
			dev_info(dp->dev, "DP: Training successful for R:%d L:%d.\n",
					cfg->rate, cfg->lanes);
			break;
		}

		dev_warn(dp->dev, "DP: Training failed for R:%d L:%d. Upgrading...\n",
			cfg->rate, cfg->lanes);
	}

	soc_dp_hw_set_msa_and_enable_video(dp, adjusted_mode, dp->link_rate,
					   dp->lane_count, st->color_format);

	mutex_unlock(&dp->mode_lock);
}

static void soc_dp_bridge_atomic_disable(struct drm_bridge *bridge,
					 struct drm_atomic_commit *state)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);

	soc_dp_link_disable(dp);

	mutex_unlock(&dp->mode_lock);
}

static int soc_dp_bridge_atomic_check(struct drm_bridge *bridge,
				      struct drm_bridge_state *bridge_state,
				      struct drm_crtc_state *crtc_state,
				      struct drm_connector_state *conn_state)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);
	struct soc_dp_bridge_state *st = to_soc_dp_bridge_state(bridge_state);
	const struct drm_display_mode *mode = &crtc_state->adjusted_mode;
	int bpp;
	u32 req_bw;
	int i;

	/* v1 supports a single output bus format -- see
	 * ->atomic_get_output_bus_fmts(). */
	st->color_format = SOC_VIDEO_RGB_8BIT;
	bpp = soc_dp_get_bpp(st->color_format);
	req_bw = soc_dp_calc_required_bw(mode, bpp);

	/*
	 * Without DPCD caps (no sink enumerated yet) we cannot pick a
	 * link config; leave idx unset and let ->atomic_enable() retry
	 * once HPD/AUX has populated dp->link.
	 */
	if (!dp->link.max_rate || !dp->link.max_num_lanes) {
		st->link_cfg_idx = -1;
		return 0;
	}

	for (i = 0; i < ARRAY_SIZE(soc_dp_link_priority_table); i++) {
		const struct soc_dp_link_config *cfg = &soc_dp_link_priority_table[i];

		if (cfg->rate > dp->link.max_rate ||
		    cfg->lanes > dp->link.max_num_lanes)
			continue;
		if (soc_dp_calc_link_capacity(cfg->rate, cfg->lanes) < req_bw)
			continue;

		st->link_cfg_idx = i;
		return 0;
	}

	drm_dbg_kms(bridge->dev,
		    "no DP link cfg fits %dx%d@%d (need %u kbit/s; sink %u kHz x %u lanes)\n",
		    mode->hdisplay, mode->vdisplay, drm_mode_vrefresh(mode),
		    req_bw, dp->link.max_rate * 10, dp->link.max_num_lanes);
	return -EINVAL;
}

static struct drm_bridge_state *
soc_dp_bridge_atomic_duplicate_state(struct drm_bridge *bridge)
{
	struct soc_dp_bridge_state *cur =
		to_soc_dp_bridge_state(drm_priv_to_bridge_state(bridge->base.state));
	struct soc_dp_bridge_state *new;

	new = kzalloc_obj(*new);
	if (!new)
		return NULL;

	__drm_atomic_helper_bridge_duplicate_state(bridge, &new->base);
	new->link_cfg_idx = cur->link_cfg_idx;
	new->color_format = cur->color_format;
	return &new->base;
}

static void
soc_dp_bridge_atomic_destroy_state(struct drm_bridge *bridge,
				   struct drm_bridge_state *state)
{
	kfree(to_soc_dp_bridge_state(state));
}

static u32 *soc_dp_bridge_atomic_get_output_bus_fmts(struct drm_bridge *bridge,
				struct drm_bridge_state *bridge_state,
				struct drm_crtc_state *crtc_state,
				struct drm_connector_state *conn_state,
				unsigned int *num_output_fmts)
{
	u32 *output_fmts;

	output_fmts = kmalloc(sizeof(*output_fmts), GFP_KERNEL);
	if (!output_fmts) {
		*num_output_fmts = 0;
		return NULL;
	}

	output_fmts[0] = MEDIA_BUS_FMT_RGB888_1X24;
	*num_output_fmts = 1;
	return output_fmts;
}

static struct drm_bridge_state *
soc_dp_bridge_atomic_reset(struct drm_bridge *bridge)
{
	struct soc_dp_bridge_state *new;

	new = kzalloc_obj(*new);
	if (!new)
		return NULL;

	__drm_atomic_helper_bridge_reset(bridge, &new->base);
	new->link_cfg_idx = -1;
	new->color_format = SOC_VIDEO_RGB_8BIT;
	return &new->base;
}

static int soc_dp_bridge_attach(struct drm_bridge *bridge,
				struct drm_encoder *encoder,
				enum drm_bridge_attach_flags flags)
{
	struct soc_dp_dev *dp = container_of(bridge, struct soc_dp_dev, bridge);

	if (!dp->next_bridge)
		return 0;

	return drm_bridge_attach(encoder, dp->next_bridge, bridge, flags);
}

/*
 * AUX-bus done-probing callback. The DRM AUX-bus framework calls this
 * after panel-edp has identified itself via DPCD and registered its
 * drm_panel. Pull the corresponding drm_bridge out of the OF graph
 * (the panel's port endpoint loops back to this controller's port@1).
 *
 * Two cases:
 *   - Synchronous: panel-edp probed inline during
 *     devm_of_dp_aux_populate_bus() inside soc_dp_bind(), before
 *     soc_dp_bind() ran drm_bridge_attach() on this controller's
 *     bridge. Just stash dp->next_bridge; soc_dp_bridge_attach()
 *     chains it when .attach fires shortly after.
 *   - Asynchronous: panel-edp probed via the deferred-probe queue
 *     well after soc_dp_bind() returned. soc_dp_bridge_attach() has
 *     already run with dp->next_bridge == NULL and the encoder's
 *     bridge chain is missing panel-edp. Late-attach panel-edp into
 *     the encoder chain here; the framework picks it up on the next
 *     modeset / drm_bridge_hpd_notify()-driven recommit, so
 *     drm_panel_prepare()/_enable() run and the panel rails plus
 *     backlight come up properly.
 */
static int soc_dp_edp_link_panel(struct drm_dp_aux *aux)
{
	struct soc_dp_dev *dp = container_of(aux, struct soc_dp_dev, aux);
	struct drm_bridge *next;
	int ret;

	next = devm_drm_of_get_bridge(dp->dev, dp->dev->of_node, 1, 0);
	if (IS_ERR(next))
		return PTR_ERR(next);

	dp->next_bridge = next;

	/*
	 * If the controller bridge has already been attached to its
	 * encoder, we are the asynchronous case -- chain panel-edp into
	 * the existing chain right now. dp->bridge.encoder is set by
	 * drm_bridge_attach() at the end of soc_dp_bind(), so its
	 * non-NULL value is the canonical "synchronously" / "late"
	 * discriminator.
	 */
	if (dp->bridge.encoder) {
		ret = drm_bridge_attach(dp->bridge.encoder, next,
					&dp->bridge,
					DRM_BRIDGE_ATTACH_NO_CONNECTOR);
		if (ret)
			return ret;

		/*
		 * Populate dp->link.{max_rate,max_num_lanes,...} from
		 * DPCD now. soc_dp_bind() only calls
		 * soc_dp_hw_read_sink_caps() when dp->connector_status
		 * reads CONNECTED, which is unreliable on the eDP path
		 * (panel-driven HPD-IN); on a fresh boot that branch is
		 * skipped and the priority-table filter in
		 * soc_dp_bridge_atomic_enable() then rejects every link
		 * config (cfg->rate > 0 > dp->link.max_rate = 0). The
		 * AUX bus has just successfully read the panel-id, so
		 * DPCD reads are known to work; do the cap read here.
		 */
		if (dp->edp_mode)
			soc_dp_hw_read_sink_caps(dp);

		drm_bridge_hpd_notify(&dp->bridge, dp->connector_status);
	}

	return 0;
}

static const struct drm_bridge_funcs soc_dp_bridge_funcs = {
	.attach				= soc_dp_bridge_attach,
	.atomic_enable			= soc_dp_bridge_atomic_enable,
	.atomic_disable			= soc_dp_bridge_atomic_disable,
	.atomic_check			= soc_dp_bridge_atomic_check,
	.atomic_duplicate_state		= soc_dp_bridge_atomic_duplicate_state,
	.atomic_destroy_state		= soc_dp_bridge_atomic_destroy_state,
	.atomic_reset			= soc_dp_bridge_atomic_reset,
	.atomic_get_output_bus_fmts	= soc_dp_bridge_atomic_get_output_bus_fmts,
	.mode_valid			= soc_dp_bridge_mode_valid,
	.detect				= soc_dp_bridge_detect,
	.edid_read			= soc_dp_bridge_edid_read,
};

static irqreturn_t soc_dp_irq_handler(int irq, void *data)
{
	struct soc_dp_dev *dp = data;
	enum drm_connector_status old_status, new_status;
	uint32_t hpd_status;
	irqreturn_t ret = IRQ_NONE;
	int idx;

	if (!drm_dev_enter(dp->drm, &idx))
		return IRQ_NONE;

	old_status = dp->connector_status;

	if (!dp->suspended) {
		new_status = soc_dp_hw_detect_hpd(dp);
		soc_dp_hw_clean_hpd(dp);
	} else {
		new_status = connector_status_disconnected;
	}

	if (new_status != old_status) {

		soc_dp_reg_read_range(dp, SOC_DPTX_HPD_IN_STATUS, &hpd_status);
		DRM_INFO("%s() hpd status 0x%x\n", __func__, hpd_status);

		dp->connector_status = new_status;
		ret = IRQ_WAKE_THREAD; /* Call hotplug_event */
	}

	drm_dev_exit(idx);
	return ret;
}

static irqreturn_t soc_dp_hotplug_event_handler(int irq, void *data)
{
	struct soc_dp_dev *dp = data;
	int idx;

	if (!drm_dev_enter(dp->drm, &idx))
		return IRQ_HANDLED;

	drm_bridge_hpd_notify(&dp->bridge, dp->connector_status);

	drm_dev_exit(idx);
	return IRQ_HANDLED;
}

static int soc_dp_resource_init(struct soc_dp_dev *dp, struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	int ret;

	{
		void __iomem *base = devm_platform_ioremap_resource(pdev, 0);

		if (IS_ERR(base))
			return dev_err_probe(dev, PTR_ERR(base),
					     "failed to map registers\n");
		dp->regs = devm_regmap_init_mmio(dev, base, &soc_dp_regmap_config);
		if (IS_ERR(dp->regs))
			return dev_err_probe(dev, PTR_ERR(dp->regs),
					     "failed to init DP regmap\n");
	}

	if (dp->dpu_id == 0) {
		dp->qos = syscon_regmap_lookup_by_phandle(pdev->dev.of_node,
							  "spacemit,qos");
		if (IS_ERR(dp->qos))
			return dev_err_probe(dev, PTR_ERR(dp->qos),
					     "failed to lookup spacemit,qos\n");

		ret = regmap_update_bits(dp->qos, SOC_DP_QOS_MUX_CTRL,
					 BIT(8), BIT(8));
		if (ret)
			return dev_err_probe(dev, ret,
					     "failed to mux DPU0 to DP/eDP0 TX\n");
	}

	/*
	 * The DP/eDP pixel clock is sourced from the PHY PLL, not from the
	 * APMU PMU clock. The APMU eDP pixel-clock mux is routed to the PHY
	 * PLL through the clock framework (clk_set_parent) in soc_dp_bind()
	 * once the PHY has registered its clock, so the controller never
	 * reprograms the pixel rate itself.
	 */
	dp->use_ext_pixel_clock = false;

	dp->irq = platform_get_irq(pdev, 0);
	if (dp->irq < 0) {
		dev_err(dev, "Failed to get IRQ\n");
		return dp->irq;
	}
	dev_info(dev, "irq %d\n", dp->irq);

	return 0;
}

static int soc_dp_dev_init(struct soc_dp_dev *dp)
{
	int ret;
	uint32_t m_isel = 0x5, m_mainsel = 0x19;
	uint32_t m_pre = 0x0, m_post = 0x2;
	uint32_t tx_mode = 0x1, tx_pre = 0x0;
	uint32_t clk_div = 24 * 1000 / 100;

	dp->ref_clk = SOC_DP_REF_CLK_24M;

	soc_dp_reg_write_range(dp, SOC_DPTX_XMIT_ENABLE, 0);
	mdelay(2);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_PD, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_PREPLL_PD, 1);
	mdelay(2);

	// Reset Controller and PHY
	soc_dp_reg_write_range(dp, SOC_DPTX_CONTROLLER_RESET, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_PHY_RESET, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_HDCP_RESET, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_RESET, 0x1);
	mdelay(5);

	// Clear Video Reset
	soc_dp_reg_write_range(dp, SOC_DPTX_CONTROLLER_RESET, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_PHY_RESET, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_HDCP_RESET, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_RESET, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_RESET, 0x0);
	mdelay(2);

	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_REPLY_EVENT_INT_STA, 1);

	soc_dp_reg_write_range(dp, SOC_DPTX_DEFAULT_FAST_LINK_TRAIN_EN, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SCRAMBLER_DISABLE, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SCALE_DOWN_MODE, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_XMIT_ENABLE, 0);

	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_STREAM_ENABLE, 0);

	// Unmask Interrupts
	soc_dp_reg_write_range(dp, SOC_DPTX_AUX_REPLY_EVENT_INT_STA_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_HDCP_INT_STA_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ILLEGAL_AUX_CMD_INT_STA_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_TYPE_C_EVENT_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_DSC_EVENT_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SDP_INT_STA_S3_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SDP_INT_STA_S2_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SDP_INT_STA_S1_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SDP_INT_STA_S0_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_FIFO_OVERFLOW_INT_STA_S3_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_FIFO_OVERFLOW_INT_STA_S2_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_FIFO_OVERFLOW_INT_STA_S1_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_VIDEO_FIFO_OVERFLOW_INT_STA_S0_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_SINK_IRQ_EVENT_MSK, 0x0);
	soc_dp_reg_write_range(dp, SOC_DPTX_HPD_INT_STA_MSK, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_HOT_PLUG_EVENT_MSK, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_HOT_UNPLUG_EVENT_MSK, 0x1);
	soc_dp_reg_write_range(dp, SOC_DPTX_SINK_UNPLUG_ERROR_EVENT_MSK, 0x0);
	mdelay(2);

	// Disable PHY SSC (Spread Spectrum Clocking)
	// soc_dp_reg_write_range(dp, SOC_DPTX_ANA_MPLL_DISABLE_SSCG, 0x1);

	// Bypass PHY busy state
	soc_dp_reg_write_range(dp, SOC_DPTX_PHY_BUSY_BYP, 0x1);

	// Enable Enhance Framing and Scale Down Mode
	soc_dp_reg_write_range(dp, SOC_DPTX_ENHANCE_FRAMING_EN, 0x1);

	// Configure PLL and Lanes
	if (dp->use_ext_pixel_clock) {
		ret = soc_dp_hw_set_pll(dp, SOC_DP_LINK_RATE_2_70, 150000);
		if (ret)
			return ret;
	} else {
		ret = soc_dp_hw_set_pll(dp, SOC_DP_LINK_RATE_2_70, 148500);
		if (ret)
			return ret;
	}

	ret = soc_dp_phy_set_lanes(dp, SOC_DP_LANE_2);
	if (ret)
		return ret;
	ret = soc_dp_phy_set_rate(dp, SOC_DP_LINK_RATE_2_70);
	if (ret)
		return ret;

	ret = soc_dp_phy_on(dp);
	if (ret)
		return ret;

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D0, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D2, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D3, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_RTCAL_FREQDIV_HBIT, (clk_div >> 8) & 0x7f);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_RTCAL_BYPASS, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_RTCAL_FREQDIV_LBIT, clk_div & 0xff);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_BG_RCAL_SEL, 0);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_RTM_D3, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_RTM_D2, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_RTM_D1, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_RTM_D0, 0);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_RTCAL_BYPASS, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_RTCAL_BYPASS, 0);
	msleep(100);

	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_PRE_D3, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_PRE_D2, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_PRE_D1, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_PRE_D0, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_DE_D3, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_DE_D2, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_DE_D1, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_DE_D0, 1);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_PRE_D3, tx_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_PRE_D2, tx_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_PRE_D1, tx_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_PRE_D0, tx_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_ISEL_DRV_D3, m_isel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_ISEL_DRV_D2, m_isel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MAINSEL_D2, m_mainsel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MAINSEL_D3, m_mainsel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_ISEL_DRV_D1, m_isel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_ISEL_DRV_D0, m_isel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_D1, m_post);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_D0, m_post);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_D3, m_post);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_POSTSEL_D2, m_post);
	soc_dp_reg_write_range(dp, SOC_DPTX_DA_TX_MAINSEL_D0_4_0, m_mainsel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MAINSEL_D1, m_mainsel);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_PRESEL_D1, m_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_PRESEL_D0, m_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_PRESEL_D3, m_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_PRESEL_D2, m_pre);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D3, tx_mode);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D2, tx_mode);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D1, tx_mode);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_MODE_D0, tx_mode);
	soc_dp_reg_write_range(dp, SOC_DPTX_ANA_TX_AUX_RX_VSEL, 0x0);

	return 0;
}

static int soc_dp_bind(struct device *dev, struct device *master, void *data)
{
	const struct soc_dp_chip_data *cd;
	int ret;
	struct soc_dp_dev *dp;
	struct drm_device *drm = (struct drm_device *)data;
	struct platform_device *pdev = to_platform_device(dev);
	struct resource *res;

	DRM_INFO("%s()\n", __func__);

	dp = devm_drm_bridge_alloc(dev, struct soc_dp_dev, bridge,
				   &soc_dp_bridge_funcs);
	if (IS_ERR(dp))
		return PTR_ERR(dp);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res)
		return dev_err_probe(dev, -ENODEV,
				     "no MMIO resource\n");
	cd = soc_dp_match_chip(res->start);
	if (!cd)
		return dev_err_probe(dev, -ENODEV,
				     "unknown DP MMIO base %pa\n", &res->start);

	dp->dev = dev;
	dp->drm = drm;
	dp->dpu_id = cd->dpu_id;
	dp->edp_mode = cd->edp_mode;
	dp->bridge.of_node = dev->of_node;
	dp->connector_status = connector_status_disconnected;
	mutex_init(&dp->mode_lock);
	dp->suspended = false;

	dp->reset = devm_reset_control_get_optional_exclusive(&pdev->dev, NULL);
	if (IS_ERR(dp->reset)) {
		ret = dev_err_probe(dev, PTR_ERR(dp->reset),
				    "failed to get reset\n");
		goto err_mutex;
	}

	dp->pxclk = devm_clk_get_optional(dev, "pxclk");
	if (IS_ERR(dp->pxclk)) {
		ret = dev_err_probe(dev, PTR_ERR(dp->pxclk),
				    "failed to get pxclk\n");
		goto err_mutex;
	}

	/*
	 * dp->phy is fetched from soc_dp_bind() below, after
	 * soc_dp_resource_init() has registered the shared "dp" regmap on
	 * this device and after devm_of_platform_populate() has instantiated
	 * the PHY child node so its driver can probe.
	 */

	if (dp->edp_mode) {
		dp->bridge.type = DRM_MODE_CONNECTOR_eDP;
		/*
		 * Declare OP_DETECT for eDP so drm_bridge_connector routes
		 * connector status through soc_dp_bridge_detect(). Without it,
		 * drm_bridge_connector_detect() hard-codes CONNECTED for any
		 * eDP connector that lacks an OP_DETECT bridge -- which would
		 * make drm_fb_helper_initial_config() try EDID/AUX on the
		 * panel-edp-less chain at master_bind time and wedge in
		 * flip_done for ~100 s. soc_dp_bridge_detect() gates the
		 * report on dp->next_bridge being set so the connector only
		 * goes CONNECTED once soc_dp_edp_link_panel() has chained
		 * panel-edp in.
		 *
		 * EDID still comes from DPCD via .edid_read.
		 */
		dp->bridge.ops = DRM_BRIDGE_OP_DETECT | DRM_BRIDGE_OP_EDID |
				 DRM_BRIDGE_OP_HPD;
	} else {
		dp->bridge.type = DRM_MODE_CONNECTOR_DisplayPort;
		/*
		 * No DRM_BRIDGE_OP_HPD: the controller's HPD interrupt is
		 * gated by the DP pixel clock, which is off while the link
		 * is idle/disconnected (exactly when a plug must be caught)
		 * and is reprogrammed on every modeset, so it cannot drive
		 * hotplug. Without OP_HPD, drm_bridge_connector_init() marks
		 * the connector POLL_CONNECT | POLL_DISCONNECT and
		 * drm_kms_helper_poll calls .detect periodically (live read).
		 */
		dp->bridge.ops = DRM_BRIDGE_OP_DETECT | DRM_BRIDGE_OP_EDID |
				 DRM_BRIDGE_OP_MODES;
	}

	if (dp->reset) {
		ret = reset_control_deassert(dp->reset);
		if (ret) {
			dev_err(dev, "failed to deassert reset: %d\n", ret);
			goto err_mutex;
		}
	}

	if (dp->pxclk) {
		ret = clk_prepare_enable(dp->pxclk);
		if (ret) {
			dev_err(dev, "failed to enable pxclk: %d\n", ret);
			goto err_reset;
		}
	}

	/*
	 * Set up the MMIO regmap and instantiate the DP PHY child *before*
	 * any DRM-side init (encoder, bridge, connector). The PHY driver's
	 * probe shares this device's regmap via dev_get_regmap(parent,
	 * "dp"), so soc_dp_resource_init() must run first; only then can
	 * devm_of_platform_populate() trigger the PHY probe, and only then
	 * will devm_phy_get() return a registered phy. Doing this ahead of
	 * the DRM-side init keeps the cleanup path simple -- a failure here
	 * does not leave a half-constructed bridge / connector behind.
	 */
	ret = soc_dp_resource_init(dp, pdev);
	if (ret)
		goto err_clk;

	ret = devm_of_platform_populate(dev);
	if (ret) {
		dev_err_probe(dev, ret, "failed to populate child devices\n");
		goto err_clk;
	}

	dp->phy = devm_phy_get(dev, "dp");
	if (IS_ERR(dp->phy)) {
		ret = dev_err_probe(dev, PTR_ERR(dp->phy),
				    "failed to get DP PHY\n");
		goto err_clk;
	}

	/*
	 * Route the APMU eDP pixel-clock mux to the PHY PLL through the clock
	 * framework. The PHY (instantiated by devm_of_platform_populate()
	 * above) exposes its PLL pixel clock as the mux's "external" parent;
	 * select it here, now that it is registered. This replaces a direct
	 * APMU mux-register write that previously reached into the clock
	 * controller through a vendor "spacemit,apmu" syscon phandle.
	 */
	if (dp->pxclk) {
		struct clk *pll;

		pll = clk_get(dev, "pll");
		if (IS_ERR(pll)) {
			ret = dev_err_probe(dev, PTR_ERR(pll),
					    "failed to get PHY pixel clock\n");
			goto err_clk;
		}

		ret = clk_set_parent(dp->pxclk, pll);
		clk_put(pll);
		if (ret) {
			dev_err(dev, "failed to route eDP pixel mux to PHY PLL: %d\n", ret);
			goto err_clk;
		}
	}

	/*
	 * AUX is used as the DDC adapter that drm_bridge_connector_init()
	 * propagates onto the connector, so bring it up before the bridge
	 * is attached.
	 */
	ret = soc_dp_aux_init(dp);
	if (ret) {
		dev_err(dev, "failed to register DP AUX: %d\n", ret);
		goto err_clk;
	}
	dp->bridge.ddc = &dp->aux.ddc;

	/*
	 * eDP: hand off panel power, enable-GPIO and backlight ownership to
	 * panel-edp via the DRM AUX-bus. devm_of_dp_aux_populate_bus()
	 * instantiates the aux-bus child node, panel-edp probes against it,
	 * and the done_probing callback stashes the resulting drm_bridge in
	 * dp->next_bridge so soc_dp_bridge_attach() can chain it. On the DP
	 * instance the controller's DT node has no aux-bus child --
	 * devm_of_dp_aux_populate_bus() then returns -ENODEV and the
	 * connector is built straight off the controller bridge by
	 * drm_bridge_connector_init() below.
	 */
	if (dp->edp_mode) {
		ret = devm_of_dp_aux_populate_bus(&dp->aux,
						  soc_dp_edp_link_panel);
		if (ret) {
			dev_err_probe(dev, ret,
				      "eDP: failed to populate aux-bus panel\n");
			goto err_aux;
		}
	}

	/*
	 * The encoder is now a passive wrapper -- atomic_enable / disable /
	 * check / mode_valid all dispatch through &dp->bridge. The encoder
	 * exists only to anchor possible_crtcs and to be the attach point
	 * drm_bridge_attach() needs.
	 */
	ret = drm_encoder_init(drm, &dp->encoder,
			&soc_dp_encoder_funcs, DRM_MODE_ENCODER_TMDS, NULL);
	if (ret) {
		dev_err(dev, "Failed to init encoder\n");
		goto err_aux;
	}

	dp->encoder.possible_crtcs = drm_of_find_possible_crtcs(drm, dev->of_node);

	/*
	 * Register the bridge on the global bridge list before attaching it
	 * to the encoder. drm_bridge_attach() warns "Missing drm_bridge_add()
	 * before attach" and silently fixes up the list when this is omitted;
	 * doing it explicitly is required by the modern bridge contract.
	 */
	ret = devm_drm_bridge_add(dev, &dp->bridge);
	if (ret) {
		dev_err(dev, "Failed to register DP bridge: %d\n", ret);
		goto err_encoder;
	}

	ret = drm_bridge_attach(&dp->encoder, &dp->bridge, NULL,
				DRM_BRIDGE_ATTACH_NO_CONNECTOR);
	if (ret) {
		dev_err(dev, "Failed to attach bridge: %d\n", ret);
		goto err_encoder;
	}

	dp->connector = drm_bridge_connector_init(drm, &dp->encoder);
	if (IS_ERR(dp->connector)) {
		ret = PTR_ERR(dp->connector);
		dev_err(dev, "Failed to init bridge connector: %d\n", ret);
		goto err_encoder;
	}

	drm_connector_attach_encoder(dp->connector, &dp->encoder);

	platform_set_drvdata(pdev, dp);

	ret = soc_dp_dev_init(dp);
	if (ret)
		goto err_encoder;

	soc_dp_link_disable(dp);

	dp->connector_status = soc_dp_hw_detect_hpd(dp);
	soc_dp_hw_clean_hpd(dp);

	if (dp->connector_status == connector_status_connected)
		soc_dp_hw_read_sink_caps(dp);

	/*
	 * eDP-async: panel-edp probe deferred (dp->next_bridge still NULL).
	 * Skip the initial HPD notification; the controller bridge has no
	 * DRM_BRIDGE_OP_DETECT (eDP is "always connected" upstream), so the
	 * connector's status comes from bridge_connector->encoder_status,
	 * which drm_bridge_hpd_notify() would set to CONNECTED. That makes
	 * drm_fb_helper_initial_config() commit an initial fbdev modeset on
	 * a chain that does not yet include panel-edp; drm_panel_prepare()
	 * never runs, the panel rails stay off, AUX/link-training fails and
	 * the commit wedges in flip_done for ~100 s -- blocking the
	 * deferred-probe worker that panel-edp (and DP1) need.
	 *
	 * soc_dp_edp_link_panel() (the AUX-bus done_probing callback)
	 * already calls drm_bridge_hpd_notify() right after late-attaching
	 * panel-edp into the chain, so the connector becomes CONNECTED via
	 * the hotplug path with the full chain in place.
	 *
	 * DP1 (!dp->edp_mode) and eDP-sync (dp->next_bridge already set)
	 * notify here as before -- the chain is complete at this point.
	 */
	if (dp->next_bridge || !dp->edp_mode)
		drm_bridge_hpd_notify(&dp->bridge, dp->connector_status);

	ret = devm_request_threaded_irq(dp->dev, dp->irq, soc_dp_irq_handler,
			soc_dp_hotplug_event_handler, 0, dev_name(dp->dev), dp);
	if (ret) {
		dev_err(dp->dev, "Failure requesting irq %d: %d.\n", dp->irq, ret);
		goto err_encoder;
	}

	return 0;

err_encoder:
	drm_encoder_cleanup(&dp->encoder);
err_aux:
	drm_dp_aux_unregister(&dp->aux);
err_clk:
	if (dp->pxclk)
		clk_disable_unprepare(dp->pxclk);
err_reset:
	if (dp->reset)
		reset_control_assert(dp->reset);
err_mutex:
	mutex_destroy(&dp->mode_lock);
	return ret;
}

static void soc_dp_unbind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);
	int ret;

	DRM_INFO("%s()\n", __func__);

	drm_dp_aux_unregister(&dp->aux);

	soc_dp_hw_disable(dp);

	drm_encoder_cleanup(&dp->encoder);

	mutex_destroy(&dp->mode_lock);

	if (dp->pxclk)
		clk_disable_unprepare(dp->pxclk);

	if (!IS_ERR_OR_NULL(dp->reset)) {
		ret = reset_control_assert(dp->reset);
		if (ret < 0) {
			DRM_INFO("Failed to assert reset\n");
		}
	}
}

static const struct component_ops soc_dp_ops = {
	.bind = soc_dp_bind,
	.unbind = soc_dp_unbind,
};

static int inno_dp_probe(struct platform_device *pdev)
{
	DRM_INFO("%s()\n", __func__);
	return component_add(&pdev->dev, &soc_dp_ops);
}

static void inno_dp_remove(struct platform_device *pdev)
{
	DRM_INFO("%s()\n", __func__);
	component_del(&pdev->dev, &soc_dp_ops);
}

static void inno_dp_shutdown(struct platform_device *pdev)
{
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);

	soc_dp_hw_disable(dp);
}

#ifdef CONFIG_PM_SLEEP

static int inno_dp_drv_pm_suspend(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);
	dp->suspended = true;
	mutex_unlock(&dp->mode_lock);

	return 0;
}

static int inno_dp_drv_pm_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);
	dp->suspended = false;
	mutex_unlock(&dp->mode_lock);

	dp->connector_status = soc_dp_hw_detect_hpd(dp);
	soc_dp_hw_clean_hpd(dp);

	if (dp->connector_status == connector_status_connected)
		soc_dp_hw_read_sink_caps(dp);

	drm_bridge_hpd_notify(&dp->bridge, dp->connector_status);

	return 0;
}

static int inno_dp_drv_pm_suspend_late(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);
	int ret;

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);

	soc_dp_hw_disable(dp);

	if (dp->pxclk)
		clk_disable_unprepare(dp->pxclk);

	if (!IS_ERR_OR_NULL(dp->reset)) {
		ret = reset_control_assert(dp->reset);
		if (ret < 0) {
			DRM_INFO("Failed to assert reset\n");
		}
	}

	mutex_unlock(&dp->mode_lock);

	return 0;
}

static int inno_dp_drv_pm_resume_early(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct soc_dp_dev *dp = platform_get_drvdata(pdev);
	int ret;

	DRM_INFO("%s()\n", __func__);

	mutex_lock(&dp->mode_lock);

	if (!IS_ERR_OR_NULL(dp->reset)) {
		ret = reset_control_deassert(dp->reset);
		if (ret < 0) {
			DRM_INFO("Failed to deassert reset\n");
		}
	}
	if (dp->pxclk)
		clk_prepare_enable(dp->pxclk);

	soc_dp_dev_init(dp);

	mutex_unlock(&dp->mode_lock);

	return 0;
}

#endif

static const struct dev_pm_ops inno_dp_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(inno_dp_drv_pm_suspend,
				inno_dp_drv_pm_resume)
	SET_LATE_SYSTEM_SLEEP_PM_OPS(inno_dp_drv_pm_suspend_late,
				     inno_dp_drv_pm_resume_early)
};

static const struct of_device_id soc_dp_match[] = {
	{ .compatible = "spacemit,k3-inno-dp" },
	{}
};
MODULE_DEVICE_TABLE(of, soc_dp_match);

struct platform_driver inno_dp_driver = {
	.probe = inno_dp_probe,
	.remove = inno_dp_remove,
	.shutdown = inno_dp_shutdown,
	.driver = {
		.name = "spacemit-inno-dp-drv",
		.of_match_table = soc_dp_match,
		.pm = &inno_dp_pm_ops,
		.suppress_bind_attrs = true,
	},
};

static int inno_dp_driver_init(void)
{
	return platform_driver_register(&inno_dp_driver);
}
late_initcall(inno_dp_driver_init);

MODULE_LICENSE("GPL");
