// SPDX-License-Identifier: GPL-2.0-only
/*
 * SpaceMiT K3 Innosilicon DisplayPort PHY driver.
 *
 * Companion to the K3 Innosilicon DP / eDP controller in
 * drivers/gpu/drm/spacemit/. The PHY's registers are interleaved with
 * the controller's own registers inside the same 16 KiB MMIO window,
 * so this driver instantiates as a child of the controller and shares
 * the controller's regmap-mmio at probe time via
 * dev_get_regmap(dev->parent, "dp").
 *
 * The split owns the dynamic PHY operations (lane count, link rate,
 * MPLL/PREPLL power, per-lane vswing / pre-emphasis); the controller
 * still owns its one-shot analog bring-up sequence and the AUX/link
 * blocks that share the same MMIO space.
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/clk-provider.h>
#include <linux/delay.h>
#include <linux/iopoll.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/phy/phy.h>
#include <linux/phy/phy-dp.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

/*
 * Subset of the K3 Innosilicon DP register map this driver writes.
 * Offsets are relative to the controller's MMIO base; sharing the
 * parent regmap means addresses are global controller offsets, not
 * PHY-local offsets.
 */
#define DPTX_PHY_CTRL			0x0100
#define DPTX_PHY_CTRL_XMIT_EN		GENMASK(20, 17)
#define DPTX_PHY_CTRL_NUM_LANES		GENMASK(6, 5)
#define DPTX_PHY_CTRL_RATE		GENMASK(1, 0)

/*
 * The per-lane vswing/preemp register packs four lanes into one word
 * as [preemp:2 | vswing:2] starting from bit 0, with a 2-bit gap
 * between lanes (so lane N starts at bit N*6 and occupies bits N*6+3
 * down to N*6+0).
 */
#define DPTX_PHY_LANE_TRIM		0x0104
#define DPTX_PHY_LANE_BITS_PER	6
#define DPTX_PHY_LANE_FIELD_MASK	0xf		/* preemp[1:0] | vswing[1:0] */
#define DPTX_PHY_LANE_VSWING_SHIFT	2		/* within the lane field */

#define DPTX_ANA_MPLL			0x0180
#define DPTX_ANA_MPLL_LOCKED		BIT(7)
#define DPTX_ANA_MPLL_PD		BIT(0)

#define DPTX_ANA_PREPLL			0x0190
#define DPTX_ANA_PREPLL_LOCKED		BIT(7)
#define DPTX_ANA_PREPLL_PD		BIT(0)

#define K3_DP_PHY_MAX_LANES		4

struct k3_inno_dp_phy {
	struct device	*dev;
	struct regmap	*regmap;
	struct phy	*phy;
	u8		lanes;	/* tracks last set_lanes; power_on uses it for XMIT_EN */
};

static int k3_inno_dp_phy_power_on(struct phy *phy)
{
	struct k3_inno_dp_phy *p = phy_get_drvdata(phy);
	u32 lane_en;
	u32 val;
	int ret;

	switch (p->lanes) {
	case 1:
		lane_en = 0x1;
		break;
	case 2:
		lane_en = 0x3;
		break;
	case 4:
	default:
		lane_en = 0xf;
		break;
	}

	ret = regmap_write_bits(p->regmap, DPTX_ANA_MPLL, DPTX_ANA_MPLL_PD, 0);
	if (ret)
		return ret;
	ret = regmap_write_bits(p->regmap, DPTX_ANA_PREPLL,
				DPTX_ANA_PREPLL_PD, 0);
	if (ret)
		return ret;
	mdelay(2);

	ret = regmap_write_bits(p->regmap, DPTX_PHY_CTRL,
				DPTX_PHY_CTRL_XMIT_EN,
				FIELD_PREP(DPTX_PHY_CTRL_XMIT_EN, lane_en));
	if (ret)
		return ret;
	mdelay(2);

	ret = regmap_read_poll_timeout(p->regmap, DPTX_ANA_MPLL, val,
				       val & DPTX_ANA_MPLL_LOCKED,
				       2000, 10 * 1000);
	if (ret) {
		dev_err(p->dev, "DP PHY core PLL lock timed out\n");
		return ret;
	}

	ret = regmap_read_poll_timeout(p->regmap, DPTX_ANA_PREPLL, val,
				       val & DPTX_ANA_PREPLL_LOCKED,
				       2000, 10 * 1000);
	if (ret)
		dev_err(p->dev, "DP PHY pixel PLL lock timed out\n");

	return ret;
}

static int k3_inno_dp_phy_power_off(struct phy *phy)
{
	struct k3_inno_dp_phy *p = phy_get_drvdata(phy);

	regmap_write_bits(p->regmap, DPTX_PHY_CTRL, DPTX_PHY_CTRL_XMIT_EN, 0);
	mdelay(2);

	regmap_write_bits(p->regmap, DPTX_ANA_PREPLL, DPTX_ANA_PREPLL_PD,
			  DPTX_ANA_PREPLL_PD);
	mdelay(2);

	return 0;
}

static int k3_inno_dp_phy_set_lanes(struct k3_inno_dp_phy *p, u8 lanes)
{
	u32 val;
	int ret;

	switch (lanes) {
	case 1:
		val = 0;
		break;
	case 2:
		val = 1;
		break;
	case 4:
		val = 2;
		break;
	default:
		return -EINVAL;
	}

	ret = regmap_write_bits(p->regmap, DPTX_PHY_CTRL,
				DPTX_PHY_CTRL_NUM_LANES,
				FIELD_PREP(DPTX_PHY_CTRL_NUM_LANES, val));
	if (ret)
		return ret;

	p->lanes = lanes;
	return 0;
}

static int k3_inno_dp_phy_set_rate(struct k3_inno_dp_phy *p,
				   unsigned int link_rate)
{
	u32 val;

	switch (link_rate) {
	case 1620:
		val = 0;
		break;
	case 2700:
		val = 1;
		break;
	case 5400:
		val = 2;
		break;
	case 8100:
		val = 3;
		break;
	default:
		return -EINVAL;
	}

	return regmap_write_bits(p->regmap, DPTX_PHY_CTRL,
				 DPTX_PHY_CTRL_RATE,
				 FIELD_PREP(DPTX_PHY_CTRL_RATE, val));
}

/*
 * Program the source-side vswing / pre-emphasis for each active lane
 * from the values link-training pulled out of the sink's adjust
 * request. The K3 PHY exposes one 6-bit vswing+preemp field per lane
 * inside a single 32-bit register at offset 0x0104; the previous
 * controller-internal helper only wrote the sink's DPCD copy and
 * left this register at its reset default, so the source TX never
 * stepped up its drive level when training requested it.
 */
static int k3_inno_dp_phy_set_voltages(struct k3_inno_dp_phy *p,
				       struct phy_configure_opts_dp *opts)
{
	u8 lane;
	int ret;

	for (lane = 0; lane < opts->lanes; lane++) {
		unsigned int shift = lane * DPTX_PHY_LANE_BITS_PER;
		u32 mask = DPTX_PHY_LANE_FIELD_MASK << shift;
		u32 val  = (((opts->voltage[lane] & 0x3) << DPTX_PHY_LANE_VSWING_SHIFT) |
			    (opts->pre[lane] & 0x3)) << shift;

		ret = regmap_write_bits(p->regmap, DPTX_PHY_LANE_TRIM, mask,
					val);
		if (ret)
			return ret;
	}

	return 0;
}

static int k3_inno_dp_phy_configure(struct phy *phy,
				    union phy_configure_opts *opts)
{
	struct k3_inno_dp_phy *p = phy_get_drvdata(phy);
	int ret;

	if (opts->dp.set_lanes) {
		ret = k3_inno_dp_phy_set_lanes(p, opts->dp.lanes);
		if (ret)
			return ret;
	}

	if (opts->dp.set_rate) {
		ret = k3_inno_dp_phy_set_rate(p, opts->dp.link_rate);
		if (ret)
			return ret;
	}

	if (opts->dp.set_voltages) {
		ret = k3_inno_dp_phy_set_voltages(p, &opts->dp);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct phy_ops k3_inno_dp_phy_ops = {
	.power_on	= k3_inno_dp_phy_power_on,
	.power_off	= k3_inno_dp_phy_power_off,
	.configure	= k3_inno_dp_phy_configure,
	.owner		= THIS_MODULE,
};

static int k3_inno_dp_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct k3_inno_dp_phy *p;
	struct phy_provider *provider;
	struct clk_hw *pxclk_hw;
	int ret;

	p = devm_kzalloc(dev, sizeof(*p), GFP_KERNEL);
	if (!p)
		return -ENOMEM;

	p->dev = dev;

	p->regmap = dev_get_regmap(dev->parent, "dp");
	if (!p->regmap)
		return dev_err_probe(dev, -ENODEV,
				     "parent DP regmap not available\n");

	p->phy = devm_phy_create(dev, dev->of_node, &k3_inno_dp_phy_ops);
	if (IS_ERR(p->phy))
		return dev_err_probe(dev, PTR_ERR(p->phy),
				     "failed to create DP PHY\n");

	phy_set_drvdata(p->phy, p);

	provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(provider))
		return dev_err_probe(dev, PTR_ERR(provider),
				     "failed to register PHY provider\n");

	/*
	 * Expose this PHY's PLL pixel clock so the APMU eDP pixel-clock mux
	 * can select it through the clock framework. Its rate is set by DP
	 * link training, not by software, and the DP controller only gates
	 * it (never reads its rate), so register it as a rate-0 clock.
	 */
	pxclk_hw = devm_clk_hw_register_fixed_rate(dev, dev_name(dev), NULL, 0, 0);
	if (IS_ERR(pxclk_hw))
		return dev_err_probe(dev, PTR_ERR(pxclk_hw),
				     "failed to register pixel clock\n");

	ret = devm_of_clk_add_hw_provider(dev, of_clk_hw_simple_get, pxclk_hw);
	if (ret)
		return dev_err_probe(dev, ret,
				     "failed to add pixel clock provider\n");

	dev_info(dev, "K3 Innosilicon DP PHY ready (parent %s)\n",
		 dev_name(dev->parent));
	return 0;
}

static const struct of_device_id k3_inno_dp_phy_of_match[] = {
	{ .compatible = "spacemit,k3-inno-dp-phy" },
	{ }
};
MODULE_DEVICE_TABLE(of, k3_inno_dp_phy_of_match);

static struct platform_driver k3_inno_dp_phy_driver = {
	.probe = k3_inno_dp_phy_probe,
	.driver = {
		.name		= "spacemit-k3-inno-dp-phy",
		.of_match_table	= k3_inno_dp_phy_of_match,
	},
};
module_platform_driver(k3_inno_dp_phy_driver);

MODULE_DESCRIPTION("SpaceMiT K3 Innosilicon DisplayPort PHY driver");
MODULE_AUTHOR("Cody Kang <cody.kang.hk@outlook.com>");
MODULE_LICENSE("GPL");
