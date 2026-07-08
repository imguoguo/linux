// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <drm/drm_atomic_helper.h>
#include <drm/drm_crtc_helper.h>
#include <drm/drm_drv.h>
#include <drm/drm_plane_helper.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/clients/drm_client_setup.h>
#include <linux/aperture.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/component.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/pm_qos.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/trace_events.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>
#include <dt-bindings/display/spacemit_dpu.h>
#include <linux/reset.h>
#include "spacemit_cmdlist.h"
#include "spacemit_dmmu.h"
#include "spacemit_drm.h"
#include "spacemit_crtc.h"
#include "dpu/dpu_saturn.h"
#include "dpu/dpu_trace.h"

static int spacemit_crtc_init(struct spacemit_crtc *a_crtc);
static int spacemit_crtc_uninit(struct spacemit_crtc *a_crtc);
static int dpu_pm_suspend(struct device *dev);
static int dpu_pm_resume(struct device *dev);

static int spacemit_crtc_atomic_check_scaling(struct drm_crtc *crtc,
					    struct drm_crtc_state *crtc_state)
{
	struct drm_plane *plane;
	struct spacemit_crtc_state *ac = to_spacemit_crtc_state(crtc_state);
	const struct drm_plane_state *pstate;
	struct spacemit_crtc_scaler *scaler = NULL;
	struct spacemit_plane_state *spacemit_pstate;
	u32 i;

	drm_atomic_crtc_state_for_each_plane_state(plane, pstate, crtc_state) {
		spacemit_pstate = to_spacemit_plane_state(pstate);

		if (spacemit_pstate->use_scl) {
			for (i = 0; i < MAX_SCALER_NUMS; i++) {
				scaler = &(ac->scalers[i]);
				if (scaler->in_use == 0x0 || scaler->rdma_id == spacemit_pstate->rdma_id) {
					scaler->in_use |= (1 << plane->index);
					scaler->rdma_id = spacemit_pstate->rdma_id;
					break;
				}
			}

			if (i == MAX_SCALER_NUMS) {
				DRM_ERROR("Exceeds the max scaler number\n");
				return -EINVAL;
			}
		}
	}

	drm_atomic_crtc_state_for_each_plane_state(plane, pstate, crtc_state) {
		spacemit_pstate = to_spacemit_plane_state(pstate);

		if (spacemit_pstate->rdma_id != RDMA_INVALID_ID) {
			for (i = 0; i < MAX_SCALER_NUMS; i++) {
				scaler = &(ac->scalers[i]);
				if (scaler->rdma_id == spacemit_pstate->rdma_id && scaler->in_use)
					spacemit_pstate->scaler_id = i;
			}
		}
	}

	return 0;
}

static void saturn_check_dpuctrl_scl_reuse(struct drm_crtc *crtc,
					    struct drm_crtc_state *crtc_state)
{
	struct drm_plane *plane;
	struct spacemit_crtc_state *spacemit_state = to_spacemit_crtc_state(crtc_state);
	struct spacemit_crtc_rdma *rdmas = spacemit_state->rdmas;
	u32 rdma_id = 0;
	u32 scl_rdma_id = 0;
	struct spacemit_crtc_scaler *scaler = NULL;
	const struct drm_plane_state *pstate;
	int i = 0;

	drm_atomic_crtc_state_for_each_plane_state(plane, pstate, crtc_state) {
		rdma_id = to_spacemit_plane_state(pstate)->rdma_id;
		if (rdma_id != RDMA_INVALID_ID)
			rdmas[rdma_id].use_cnt++;
	}

	//TODO: SCALER_NUMS for different IP
	for (i = 0; i < MAX_SCALER_NUMS; i++) {
		scaler = &(spacemit_state->scalers[i]);

		if (scaler->in_use)
			spacemit_state->scl_rdma_id[i] = scaler->rdma_id;
		else
			spacemit_state->scl_rdma_id[i] = RDMA_INVALID_ID;

		if (rdmas[scl_rdma_id].use_cnt > 1)
			spacemit_state->scl_rdma_reuse[i] = true;
	}
}

static int spacemit_crtc_atomic_check_fbmem(struct drm_crtc *crtc,
					    struct drm_crtc_state *crtc_state)
{
	struct spacemit_crtc_rdma *rdmas = to_spacemit_crtc_state(crtc_state)->rdmas;
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct spacemit_drm_private *priv = crtc->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;

	const struct drm_plane_state *pstate;
	struct drm_plane *plane;
	/* Calc each rdma required fbc mem size */
	drm_atomic_crtc_state_for_each_plane_state(plane, pstate, crtc_state) {
		u32 rdma_id = to_spacemit_plane_state(pstate)->rdma_id;
		u32 layer_fbcmem_size = to_spacemit_plane_state(pstate)->fbcmem_size;

		if (rdma_id != RDMA_INVALID_ID) {
			if (rdmas[rdma_id].mode == UP_DOWN)
				rdmas[rdma_id].fbcmem.size = max(layer_fbcmem_size,
								 rdmas[rdma_id].fbcmem.size);
			else
				rdmas[rdma_id].fbcmem.size += layer_fbcmem_size;
		}
	}

	/* Adjust each rdma's fbcmem layout */
	return a_crtc->core->adjust_rdma_fbcmem(hwdev, rdmas);

}

static void spacemit_crtc_atomic_enable(struct drm_crtc *crtc,
				   struct drm_atomic_commit *old_state)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	int idx;

	if (!drm_dev_enter(crtc->dev, &idx))
		return;

	DRM_INFO("%s(power on)\n", __func__);
	trace_spacemit_crtc_atomic_enable(a_crtc->dev_id);

	/*
	 * U-Boot brings up the DPU to display a boot logo and leaves the
	 * pipeline running when it hands control to the kernel. If the
	 * kernel programs its first frame on top of that in-flight pipeline
	 * the timing generator and RDMA collide and the very first IRQ
	 * delivery fires a spurious UNDERRUN ("Under Run! DPU_Mclk = 0,
	 * DPU BW = 0").
	 *
	 * Avoid that by resetting the DPU on the first atomic_enable for
	 * each CRTC. Enable runtime PM, then pm_get / pm_put once: the
	 * runtime-PM resume callback (dpu_pm_resume) deasserts the DPU's
	 * shared reset lines and enables its clocks; the suspend callback
	 * (dpu_pm_suspend) disables clocks and asserts the reset lines,
	 * which hard-stops the U-Boot pipeline. The subsequent
	 * if (!a_crtc->power_on) path below then resumes the device
	 * normally for the real first frame.
	 */
	if (unlikely(a_crtc->first_modeset)) {
		spacemit_dpu_power_enable(a_crtc, true);
		spacemit_dpu_power_enable(a_crtc, false);
		msleep(10);
		a_crtc->first_modeset = false;
	}

	if (!a_crtc->power_on) {
		spacemit_dpu_power_enable(a_crtc, true);
		dpu_pm_resume(a_crtc->dev);

		/*
		 * Let the DPU settle after its reset lines are deasserted and
		 * clocks re-enabled in dpu_pm_resume(), before programming the
		 * first frame. The first modeset above already holds msleep(10);
		 * every re-enable (e.g. a DP hot-plug disable/enable cycle, which
		 * now happens because HPD detection works) skipped it and raced
		 * the CFG_RDY handshake, occasionally wedging flip_done after
		 * several cycles.
		 */
		msleep(10);
	}

	drm_crtc_vblank_on(&a_crtc->crtc);

	spacemit_crtc_init(a_crtc);
	a_crtc->rpm_status = true;

	drm_dev_exit(idx);
}

static void spacemit_crtc_atomic_disable(struct drm_crtc *crtc,
				    struct drm_atomic_commit *old_state)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct drm_device *drm = a_crtc->crtc.dev;
	int idx;

	if (!drm_dev_enter(drm, &idx))
		return;

	DRM_INFO("%s(power off)\n", __func__);
	trace_spacemit_crtc_atomic_disable(a_crtc->dev_id);

	spacemit_crtc_uninit(a_crtc);

	drm_crtc_vblank_off(&a_crtc->crtc);

	dpu_pm_suspend(a_crtc->dev);
	spacemit_dpu_power_enable(a_crtc, false);

	spin_lock_irq(&drm->event_lock);
	if (crtc->state->event) {
		drm_crtc_send_vblank_event(crtc, crtc->state->event);
		crtc->state->event = NULL;
	}
	spin_unlock_irq(&drm->event_lock);
	a_crtc->rpm_status = false;

	drm_dev_exit(idx);
}

static int spacemit_crtc_atomic_check(struct drm_crtc *crtc,
						struct drm_atomic_commit *atomic_state)
{
	struct drm_crtc_state *state = drm_atomic_get_new_crtc_state(atomic_state, crtc);
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	int ret = 0;

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_atomic_check(a_crtc->dev_id);

	ret = spacemit_crtc_atomic_check_scaling(crtc, state);
	if (ret) {
		DRM_ERROR("crtc_id=%u check_scaling FAILED ret=%d\n",
			  crtc->base.id, ret);
		return -EINVAL;
	}

	if (spacemit_crtc_atomic_check_fbmem(crtc, state)) {
		DRM_ERROR("Failed to satisfy fbcmem size for all rdmas!\n");
		return -EINVAL;
	}

	saturn_check_dpuctrl_scl_reuse(crtc, state);

	return ret;
}

static void spacemit_crtc_atomic_begin(struct drm_crtc *crtc,
				  struct drm_atomic_commit *state)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct videomode vm;
	int idx;

	if (!drm_dev_enter(crtc->dev, &idx))
		return;

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_atomic_begin(a_crtc->dev_id);
	drm_display_mode_to_videomode(&crtc->mode, &vm);
	if (a_crtc->vrr_vfp != vm.vfront_porch)
		a_crtc->vrr_vfp = vm.vfront_porch;

	drm_dev_exit(idx);
}

#define VSYNC_PERIOD_VARIANCE_NS		2000000

static void spacemit_wait_earliest_process_time(int32_t vrefresh, uint64_t expected_present_time)
{

	ktime_t now;
	int64_t present_time_adjust, delay_until_process;
	int64_t vsync_period_ns = mult_frac(1000, 1000 * 1000, vrefresh);

	present_time_adjust = expected_present_time - (vsync_period_ns - VSYNC_PERIOD_VARIANCE_NS);
	if (present_time_adjust <= 0) {
		// Don't need to wait
		return;
	}

	now = ktime_get();
	delay_until_process = (int64_t)ktime_us_delta(present_time_adjust, now);
	if (delay_until_process > 0) {
		int32_t max_delay_us = (10 * vsync_period_ns) / 1000;

		if (delay_until_process > max_delay_us) {
			delay_until_process = max_delay_us;
			pr_warn("expected present time seems incorrect(now %llu, earliest %llu)\n",
					now, present_time_adjust);
		}
		usleep_range(delay_until_process, delay_until_process + 10);
	}
}

static void spacemit_crtc_atomic_flush(struct drm_crtc *crtc,
				  struct drm_atomic_commit *state)

{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct spacemit_crtc_state *new_state = to_spacemit_crtc_state(crtc->state);
	struct drm_crtc_state *old_state = drm_atomic_get_old_crtc_state(state, crtc);
	int idx;

	if (!drm_dev_enter(crtc->dev, &idx))
		return;

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_atomic_flush(a_crtc->dev_id);

	if (new_state->expected_present_time != 0) {
		int32_t vrefresh = drm_mode_vrefresh(&old_state->mode);

		if (vrefresh == 0) {
			/* decon just be enabled */
			vrefresh = drm_mode_vrefresh(&crtc->state->mode);
		}

		spacemit_wait_earliest_process_time(vrefresh, new_state->expected_present_time);
	}

	spacemit_crtc_run(crtc, old_state);

	drm_dev_exit(idx);
}

static struct drm_crtc_state *spacemit_crtc_duplicate_state(struct drm_crtc *crtc)
{
	struct spacemit_crtc_state *state;
	struct spacemit_drm_private *priv = crtc->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u8 n_rdma, i;

	if (WARN_ON(!crtc->state))
		return NULL;

	state = kzalloc(sizeof(*state), GFP_KERNEL);
	if (!state)
		return NULL;

	__drm_atomic_helper_crtc_duplicate_state(crtc, &state->base);
	memset(&state->scalers, 0x0, sizeof(struct spacemit_crtc_scaler) * MAX_SCALER_NUMS);

	n_rdma = hwdev->rdma_nums;
	state->rdmas = kzalloc(sizeof(struct spacemit_crtc_rdma) * n_rdma, GFP_KERNEL);
	if (!state->rdmas) {
		kfree(state);
		return NULL;
	}

	/* Rdma use UP_DOWN mode by default */
	for (i = 0; i < n_rdma; i++) {
		state->rdmas[i].mode = UP_DOWN;
		state->rdmas[i].in_use = false;
	}

	for (i = 0; i < MAX_CL_NUM; i++) {
		state->cl[i].index = i;
		state->cl[i].type = CMDLIST_CRTC;
	}

	return &state->base;
}

static void spacemit_crtc_destroy_state(struct drm_crtc *crtc,
				struct drm_crtc_state *state)
{
	struct spacemit_crtc_state *spacemit_state = NULL;
	struct spacemit_crtc *a_crtc = NULL;
	int i = 0;

	if (state) {
		spacemit_state = to_spacemit_crtc_state(state);
		__drm_atomic_helper_crtc_destroy_state(state);
		a_crtc = to_spacemit_crtc(crtc);
		for (i = 0; i < MAX_CL_NUM; i++) {
			if (spacemit_state->cl[i].va)
				dma_free_coherent(a_crtc->dev, spacemit_state->cl[i].size,
					spacemit_state->cl[i].va, spacemit_state->cl[i].pa);
		}
		kfree(spacemit_state->rdmas);
		kfree(spacemit_state);
	}
}

static void spacemit_crtc_reset(struct drm_crtc *crtc)
{
	struct spacemit_crtc_state *state =
		kzalloc(sizeof(*state), GFP_KERNEL);
	struct spacemit_drm_private *priv = crtc->dev->dev_private;
	struct spacemit_hw_device *hwdev = priv->hwdev;
	u8 n_rdma;

	if (crtc->state)
		spacemit_crtc_destroy_state(crtc, crtc->state);

	__drm_atomic_helper_crtc_reset(crtc, &state->base);

	n_rdma = hwdev->rdma_nums;
	state->rdmas = kzalloc(sizeof(struct spacemit_crtc_rdma) * n_rdma, GFP_KERNEL);
	if (!state->rdmas) {
		DRM_ERROR("Failed to allocate memory of struct spacemit_crtc_rdma!\n");
		return;
	}

}

static int spacemit_crtc_enable_vblank(struct drm_crtc *crtc)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_enable_vblank(a_crtc->dev_id);

	if (a_crtc->core && a_crtc->core->enable_vsync)
		a_crtc->core->enable_vsync(a_crtc);

	return 0;
}

static void spacemit_crtc_disable_vblank(struct drm_crtc *crtc)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_disable_vblank(a_crtc->dev_id);

	if (a_crtc->core && a_crtc->core->disable_vsync)
		a_crtc->core->disable_vsync(a_crtc);
}

static int spacemit_crtc_atomic_set_property(struct drm_crtc *crtc,
				   struct drm_crtc_state *state,
				   struct drm_property *property,
				   uint64_t val)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct spacemit_crtc_state *s = to_spacemit_crtc_state(state);

	DRM_DEBUG("%s() name = %s, val = %llu\n",
		  __func__, property->name, val);

	if (property == a_crtc->expected_present_time) {
		s->expected_present_time = val;
		return 0;
	} else if (property == a_crtc->post_scaler_property) {
		s->post_scl_on = val;
		return 0;
	} else {
		DRM_ERROR("property %s is invalid\n", property->name);
		return -EINVAL;
	}

	return 0;
}

static int spacemit_crtc_atomic_get_property(struct drm_crtc *crtc,
					  const struct drm_crtc_state *state,
					  struct drm_property *property,
					  u64 *val)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	struct spacemit_crtc_state *s = to_spacemit_crtc_state(state);

	DRM_DEBUG("%s() name = %s\n", __func__, property->name);

	if (property == a_crtc->post_scaler_property) {
		*val = s->post_scl_on;
	} else if (property == a_crtc->expected_present_time) {
		*val = (a_crtc->expected_present_time) ? a_crtc->expected_present_time->base.id : 0;
	} else {
		DRM_ERROR("property %s is invalid\n", property->name);
		return -EINVAL;
	}

	return 0;
}

static const struct drm_crtc_helper_funcs spacemit_crtc_helper_funcs = {
	.atomic_check = spacemit_crtc_atomic_check,
	.atomic_begin = spacemit_crtc_atomic_begin,
	.atomic_flush = spacemit_crtc_atomic_flush,
	.atomic_enable = spacemit_crtc_atomic_enable,
	.atomic_disable = spacemit_crtc_atomic_disable,
};

static const struct drm_crtc_funcs spacemit_crtc_funcs = {
	.atomic_get_property = spacemit_crtc_atomic_get_property,
	.atomic_set_property = spacemit_crtc_atomic_set_property,
	.destroy = drm_crtc_cleanup,
	.set_config = drm_atomic_helper_set_config,
	.page_flip = drm_atomic_helper_page_flip,
	.reset = spacemit_crtc_reset,
	.atomic_duplicate_state = spacemit_crtc_duplicate_state,
	.atomic_destroy_state = spacemit_crtc_destroy_state,
	.enable_vblank = spacemit_crtc_enable_vblank,
	.disable_vblank = spacemit_crtc_disable_vblank,
};

static int spacemit_crtc_create_properties(struct drm_crtc *crtc)
{
	struct drm_property *prop;
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);
	int ret = 0;

	DRM_DEBUG("%s()\n", __func__);

	prop = drm_property_create_range(crtc->dev, 0,
			"expected_present_time", 0, ULLONG_MAX);
	if (!prop) {
		DRM_ERROR("expected_present_time failed %d\n", ret);
		return -ENOMEM;
	}
	drm_object_attach_property(&crtc->base, prop, 0);
	a_crtc->expected_present_time = prop;

	prop = drm_property_create_bool(crtc->dev, DRM_MODE_PROP_ATOMIC,
			"pos_scl");
	if (!prop) {
		DRM_ERROR("pos_scl %d\n", ret);
		return -ENOMEM;
	}
	drm_object_attach_property(&crtc->base, prop, 0);
	a_crtc->post_scaler_property = prop;

	return 0;
}

static int spacemit_crtc_bind_init(struct drm_device *drm, struct drm_crtc *crtc,
			 struct drm_plane *primary, struct device_node *port)
{
	int err;

	/*
	 * set crtc port so that drm_of_find_possible_crtcs call works
	 */
	of_node_put(port);
	crtc->port = port;

	err = drm_crtc_init_with_planes(drm, crtc, primary, NULL,
					&spacemit_crtc_funcs, NULL);
	if (err) {
		DRM_ERROR("failed to init crtc.\n");
		return err;
	}

	drm_mode_crtc_set_gamma_size(crtc, 256);

	drm_crtc_helper_add(crtc, &spacemit_crtc_helper_funcs);

	spacemit_crtc_create_properties(crtc);

	DRM_INFO("%s() ok\n", __func__);
	return 0;
}

int spacemit_crtc_run(struct drm_crtc *crtc,
		struct drm_crtc_state *old_state)
{
	struct spacemit_crtc *a_crtc = to_spacemit_crtc(crtc);

	DRM_DEBUG("%s()\n", __func__);
	trace_spacemit_crtc_run(a_crtc->dev_id);

	if (a_crtc->core && a_crtc->core->run)
		a_crtc->core->run(crtc, old_state);

	return 0;
}

static int spacemit_crtc_init(struct spacemit_crtc *a_crtc)
{
	trace_spacemit_crtc_init(a_crtc->dev_id);

	if (a_crtc->core && a_crtc->core->init)
		a_crtc->core->init(a_crtc);

	a_crtc->is_1st_f = true;

	return 0;
}

static int spacemit_crtc_uninit(struct spacemit_crtc *a_crtc)
{
	trace_spacemit_crtc_uninit(a_crtc->dev_id);

	if (a_crtc->core && a_crtc->core->uninit)
		a_crtc->core->uninit(a_crtc);

	return 0;
}

static irqreturn_t spacemit_dpu_isr(int irq, void *data)
{
	struct spacemit_crtc *a_crtc = data;
	int idx;

	if (!drm_dev_enter(a_crtc->crtc.dev, &idx))
		return IRQ_NONE;

	if (a_crtc->core && a_crtc->core->online_isr)
		a_crtc->core->online_isr(a_crtc);

	drm_dev_exit(idx);

	return IRQ_HANDLED;
}

static int spacemit_dpu_irqs_init(struct spacemit_crtc *a_crtc,
				struct device_node *np, struct platform_device *pdev)
{
	int err;

	int irq_online, irq_offline;

	DRM_INFO("%s()\n", __func__);
	/*request irq*/
	irq_online = platform_get_irq_byname(pdev, "online");
	irq_offline = platform_get_irq_byname(pdev, "offline");
	if ((irq_online < 0) && (irq_offline < 0)) {
		DRM_ERROR("failed to get ONLINE irq number %d\n", irq_online);
		DRM_ERROR("failed to get OFFLINE irq number %d\n", irq_offline);
		// return -EINVAL;
	}
	DRM_DEBUG("dpu online_irq = %d\n", irq_online);
	DRM_DEBUG("dpu offline_irq = %d\n", irq_offline);

	if (irq_online > 0) {
		err = devm_request_irq(&pdev->dev, irq_online, spacemit_dpu_isr,
				       0, "DPU_ONLINE", a_crtc);
		if (err) {
			DRM_ERROR("error: dpu request online irq failed\n");
			return -EINVAL;
		}
	}

	if (irq_offline > 0) {
		err = devm_request_irq(&pdev->dev, irq_offline, spacemit_dpu_isr,
				       0, "DPU_OFFLINE", a_crtc);
		if (err) {
			DRM_ERROR("error: dpu request offline irq failed\n");
			return -EINVAL;
		}
	}

	return 0;
}

static const struct regmap_config spacemit_dpu_regmap_config = {
	.name		= "dpu",
	.reg_bits	= 32,
	.val_bits	= 32,
	.reg_stride	= 4,
	.max_register	= 0x53ffc,	/* 0x54000-byte DPU MMIO from binding */
};

static int spacemit_drm_master_bind(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);
	struct spacemit_crtc *a_crtc = priv->a_crtc;
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;
	struct device_node *ports = NULL, *port = NULL;
	struct drm_plane *plane;
	struct drm_device *drm;
	int ret;

	DRM_DEBUG("%s()\n", __func__);

	ret = aperture_remove_all_conflicting_devices(spacemit_drm_drv.name);
	if (ret) {
		DRM_ERROR("Failed to remove existing framebuffers - %d.\n", ret);
		return ret;
	}

	drm = drm_dev_alloc(&spacemit_drm_drv, dev);
	if (IS_ERR(drm))
		return PTR_ERR(drm);

	priv->ddev = drm;
	drm->dev_private = priv;

	spacemit_drm_mode_config_init(drm);

	/* DPU CRTC bring-up (formerly via component framework). */
	ret = spacemit_dpu_irqs_init(a_crtc, np, pdev);
	if (ret)
		goto err_dc;

	ret = devm_pm_runtime_enable(dev);
	if (ret) {
		ret = dev_err_probe(dev, ret, "failed to enable runtime PM\n");
		goto err_dc;
	}

	timer_setup(&a_crtc->cfg_rdy_timer, spacemit_cfg_rdy_timer_handler, 0);

	plane = spacemit_plane_init(drm, a_crtc);
	if (IS_ERR_OR_NULL(plane)) {
		ret = PTR_ERR(plane);
		goto err_dc;
	}

	ports = of_get_child_by_name(np, "ports");
	if (!ports) {
		DRM_ERROR("CRTC %pOF has no ports node\n", np);
		ret = -EINVAL;
		goto err_dc;
	}

	port = of_get_child_by_name(ports, "port");
	if (!port) {
		DRM_ERROR("CRTC %pOF has no port@X node\n", np);
		ret = -EINVAL;
		goto err_ports_put;
	}

	ret = spacemit_crtc_bind_init(drm, &a_crtc->crtc, plane, port);
	of_node_put(port);
	of_node_put(ports);
	port = NULL;
	ports = NULL;
	if (ret)
		goto err_dc;

	/* Bind downstream components (DP/eDP bridge). */
	ret = component_bind_all(dev, drm);
	if (ret) {
		DRM_ERROR("failed to bind all component.\n");
		goto err_crtc_cleanup;
	}

	ret = drm_vblank_init(drm, drm->mode_config.num_crtc);
	if (ret) {
		DRM_ERROR("failed to initialize vblank.\n");
		goto err_unbind_all;
	}

	drm_mode_config_reset(drm);
	drm_kms_helper_poll_init(drm);
	drm_helper_hpd_irq_event(drm);

	ret = drm_dev_register(drm, 0);
	if (ret < 0)
		goto err_kms_helper_poll_fini;
	drm_client_setup(drm, NULL);

	DRM_INFO("dpu driver probe success\n");

	return 0;

err_kms_helper_poll_fini:
	drm_kms_helper_poll_fini(drm);
err_unbind_all:
	component_unbind_all(dev, drm);
err_crtc_cleanup:
	drm_crtc_cleanup(&a_crtc->crtc);
err_ports_put:
	of_node_put(port);
	of_node_put(ports);
err_dc:
	drm_mode_config_cleanup(drm);
	drm_dev_put(drm);
	return ret;
}

static void spacemit_drm_master_unbind(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);
	struct drm_device *drm = priv->ddev;
	struct spacemit_crtc *a_crtc = priv->a_crtc;

	DRM_DEBUG("%s()\n", __func__);

	drm_dev_unregister(drm);
	drm_kms_helper_poll_fini(drm);
	drm_crtc_cleanup(&a_crtc->crtc);
	component_unbind_all(dev, drm);
	drm_mode_config_cleanup(drm);
	drm_dev_put(drm);
}

const struct component_master_ops spacemit_drm_master_ops = {
	.bind = spacemit_drm_master_bind,
	.unbind = spacemit_drm_master_unbind,
};

static int spacemit_dpu_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct spacemit_hw_device *hwdev_template;
	struct spacemit_drm_private *priv;
	struct spacemit_crtc *a_crtc;
	struct device_node *np = dev->of_node;
	struct resource *r;
	u32 dpu_out_format;
	int ret;

	DRM_INFO("%s()\n", __func__);
	if (!np) {
		DRM_DEV_ERROR(dev, "can't find dpu devices\n");
		return -ENODEV;
	}

	ret = dma_coerce_mask_and_coherent(dev, DMA_BIT_MASK(64));
	if (ret)
		DRM_ERROR("dma_set_mask_and_coherent failed (%d)\n", ret);

	/* Top-level drm-card private state. */
	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->dev = dev;
	dev_set_drvdata(dev, priv);

	/*
	 * The match table entry is shared SoC-wide const state; the MMIO
	 * mapping and a few reboot/state bits are per-instance and must
	 * not be written through to the template. Clone the template into
	 * a writable per-probe copy.
	 */
	hwdev_template = of_device_get_match_data(dev);
	if (!hwdev_template)
		return -ENODEV;
	priv->hwdev = devm_kmemdup(dev, hwdev_template,
				   sizeof(*hwdev_template), GFP_KERNEL);
	if (!priv->hwdev)
		return -ENOMEM;

	priv->cmdlist_groups = devm_kcalloc(dev,
				priv->hwdev->rdma_nums + priv->hwdev->crtc_nums,
				sizeof(*priv->cmdlist_groups), GFP_KERNEL);
	if (!priv->cmdlist_groups)
		return -ENOMEM;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r)
		return -EINVAL;
	priv->hwdev->phy_addr = r->start;
	priv->hwdev->base = devm_ioremap_resource(dev, r);
	if (IS_ERR(priv->hwdev->base))
		return PTR_ERR(priv->hwdev->base);

	priv->hwdev->regmap = devm_regmap_init_mmio(dev, priv->hwdev->base,
						    &spacemit_dpu_regmap_config);
	if (IS_ERR(priv->hwdev->regmap))
		return dev_err_probe(dev, PTR_ERR(priv->hwdev->regmap),
				     "failed to init DPU regmap\n");

	/* Per-DPU CRTC state. */
	a_crtc = devm_kzalloc(dev, sizeof(*a_crtc), GFP_KERNEL);
	if (!a_crtc)
		return -ENOMEM;
	a_crtc->dev = dev;
	a_crtc->power_on = false;
	a_crtc->first_modeset = true;
	a_crtc->rpm_status = false;
	priv->a_crtc = a_crtc;

	a_crtc->mclk_reset = devm_reset_control_get_optional_exclusive(&pdev->dev, "mclk");
	if (IS_ERR_OR_NULL(a_crtc->mclk_reset))
		DRM_DEV_DEBUG(dev, "not found mclk_reset\n");
	a_crtc->esc_reset = devm_reset_control_get_optional_exclusive(&pdev->dev, "esc");
	if (IS_ERR_OR_NULL(a_crtc->esc_reset))
		DRM_DEV_DEBUG(dev, "not found esc_reset\n");
	a_crtc->lcd_reset = devm_reset_control_get_optional_exclusive(&pdev->dev, "lcd");
	if (IS_ERR_OR_NULL(a_crtc->lcd_reset))
		DRM_DEV_DEBUG(dev, "not found lcd_reset\n");
	a_crtc->aclk_reset = devm_reset_control_get_optional_exclusive(&pdev->dev, "aclk");
	if (IS_ERR_OR_NULL(a_crtc->aclk_reset))
		DRM_DEV_DEBUG(dev, "not found aclk_reset\n");
	a_crtc->dsc_reset = devm_reset_control_get_optional_exclusive(&pdev->dev, "dsc");
	if (IS_ERR_OR_NULL(a_crtc->dsc_reset))
		DRM_DEV_DEBUG(dev, "not found dsc_reset\n");

	/*
	 * K3 wires both DPU instances to the same online compositor
	 * type (COMPOSER1) in the Saturn IRQ tables; there is no
	 * "instance index 0 vs 1" notion at this layer. The legacy
	 * "pipeline-id" dtsi property encoded this constant in DT,
	 * which was both non-standard and a footgun (both DPUs had to
	 * carry the same value, otherwise IRQ-bit lookups would
	 * silently return zero). Pin it in the driver instead.
	 */
	a_crtc->dev_id = COMPOSER1;

	if (of_property_read_u32(np, "out-format", &dpu_out_format))
		dpu_out_format = OUTFMT_RGB888;
	a_crtc->out_format = dpu_out_format;

	/*
	 * K3 ships exactly one DPU IP family (Saturn), so wire the ops
	 * table directly; the vendor string-keyed ops registry only ever
	 * held this one entry.
	 */
	a_crtc->core = &dpu_saturn_ops;

	/* Clk dts nodes must be parsed in head of pm_runtime_xxx */
	if (a_crtc->core && a_crtc->core->parse_dt)
		a_crtc->core->parse_dt(a_crtc, np);

	return spacemit_drm_of_component_probe(dev);
}

static void spacemit_dpu_remove(struct platform_device *pdev)
{
	component_master_del(&pdev->dev, &spacemit_drm_master_ops);
}

static void spacemit_dpu_shutdown(struct platform_device *pdev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(&pdev->dev);
	struct drm_device *drm = priv->ddev;

	if (!drm) {
		DRM_WARN("drm device is not available, no shutdown\n");
		return;
	}

	priv->hwdev->reboot_flag = 1;
	drm_atomic_helper_shutdown(drm);
}

static int __maybe_unused dpu_pm_suspend(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);
	struct spacemit_crtc *a_crtc = priv->a_crtc;
	int result;

	DRM_DEBUG("%s()\n", __func__);

	if (a_crtc->core && a_crtc->core->disable_clk)
		a_crtc->core->disable_clk(a_crtc);

	if (!IS_ERR_OR_NULL(a_crtc->lcd_reset)) {
		result = reset_control_assert(a_crtc->lcd_reset);
		if (result < 0)
			DRM_INFO("Failed to assert lcd_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->esc_reset)) {
		result = reset_control_assert(a_crtc->esc_reset);
		if (result < 0)
			DRM_INFO("Failed to assert esc_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->mclk_reset)) {
		result = reset_control_assert(a_crtc->mclk_reset);
		if (result < 0)
			DRM_INFO("Failed to assert mclk_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->aclk_reset)) {
		result = reset_control_assert(a_crtc->aclk_reset);
		if (result < 0)
			DRM_INFO("Failed to assert aclk_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->dsc_reset)) {
			result = reset_control_assert(a_crtc->dsc_reset);
			if (result < 0)
				DRM_INFO("Failed to assert dsc_reset: %d\n", result);
	}

	return 0;
}

static int __maybe_unused dpu_pm_resume(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);
	struct spacemit_crtc *a_crtc = priv->a_crtc;
	int result;

	DRM_DEBUG("%s()\n", __func__);

	if (!IS_ERR_OR_NULL(a_crtc->mclk_reset)) {
		result = reset_control_deassert(a_crtc->mclk_reset);
		if (result < 0)
			DRM_INFO("Failed to deassert mclk_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->esc_reset)) {
		result = reset_control_deassert(a_crtc->esc_reset);
		if (result < 0)
			DRM_INFO("Failed to deassert esc_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->lcd_reset)) {
		result = reset_control_deassert(a_crtc->lcd_reset);
		if (result < 0)
			DRM_INFO("Failed to deassert lcd_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->aclk_reset)) {
		result = reset_control_deassert(a_crtc->aclk_reset);
		if (result < 0)
			DRM_INFO("Failed to deassert aclk_reset: %d\n", result);
	}
	if (!IS_ERR_OR_NULL(a_crtc->dsc_reset)) {
			result = reset_control_deassert(a_crtc->dsc_reset);
			if (result < 0)
				DRM_INFO("Failed to deassert dsc_reset: %d\n", result);
	}

	if (a_crtc->core && a_crtc->core->enable_clk)
		a_crtc->core->enable_clk(a_crtc);

	return 0;
}

static int __maybe_unused dpu_rt_pm_suspend(struct device *dev)
{
	// struct spacemit_crtc *a_crtc = dev_get_drvdata(dev);

	DRM_DEBUG("%s() \n", __func__);

	return 0;
}

static int __maybe_unused dpu_rt_pm_resume(struct device *dev)
{
	// struct spacemit_crtc *a_crtc = dev_get_drvdata(dev);

	DRM_DEBUG("%s() \n", __func__);

	return 0;
}

static int __maybe_unused spacemit_dpu_sys_suspend(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);

	return drm_mode_config_helper_suspend(priv->ddev);
}

static int __maybe_unused spacemit_dpu_sys_resume(struct device *dev)
{
	struct spacemit_drm_private *priv = dev_get_drvdata(dev);

	return drm_mode_config_helper_resume(priv->ddev);
}

static const struct dev_pm_ops dpu_pm_ops = {
	SET_SYSTEM_SLEEP_PM_OPS(spacemit_dpu_sys_suspend,
				spacemit_dpu_sys_resume)
	SET_RUNTIME_PM_OPS(dpu_rt_pm_suspend,
			dpu_rt_pm_resume,
			NULL)
};

static const struct of_device_id dpu_match_table[] = {
	{
		.compatible = "spacemit,k3-saturn-dpu",
		.data = &spacemit_dp_devices[SATURN_HEE],
	},
	{},
};
MODULE_DEVICE_TABLE(of, dpu_match_table);

static struct platform_driver spacemit_dpu_driver = {
	.probe = spacemit_dpu_probe,
	.remove = spacemit_dpu_remove,
	.shutdown = spacemit_dpu_shutdown,
	.driver = {
		.name = "spacemit-dpu-drv",
		.of_match_table = dpu_match_table,
		.pm = &dpu_pm_ops,
		.suppress_bind_attrs = true,
	},
};
module_platform_driver(spacemit_dpu_driver);

MODULE_DESCRIPTION("Spacemit DRM KMS Master Driver");
MODULE_LICENSE("GPL v2");
