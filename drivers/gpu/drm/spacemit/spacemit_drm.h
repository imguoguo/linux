/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#ifndef _SPACEMIT_DRM_H_
#define _SPACEMIT_DRM_H_

#include <drm/drm_print.h>
#include <drm/drm_atomic.h>
#include <drm/drm_drv.h>
#include <drm/drm_vblank.h>
#include <drm/drm_probe_helper.h>
#include <linux/dma-mapping.h>
#include "spacemit_dmmu.h"

/**
 * struct spacemit_drm_private - Top-level driver state for the spacemit DRM card
 * @ddev:             DRM device created by drm_dev_alloc() in the component
 *                    master bind callback.
 * @dev:              Underlying platform-device pointer for the DPU. This is
 *                    the same device that hosts the spacemit-dpu platform
 *                    driver - the DPU is the drm subsystem root.
 * @a_crtc:           Per-DPU CRTC state. Allocated alongside @hwdev in probe
 *                    and bound to the drm device in the master bind callback.
 * @hwdev:            Per-board static description of the Saturn DPU hardware
 *                    (channel counts, register strides, etc.); the table that
 *                    distinguishes K3 from any future Saturn-derived SoC lives
 *                    here.
 * @cmdlist_groups:   Pool of per-pipeline cmdlist groups; each CRTC borrows one
 *                    while building register-write batches for atomic_flush.
 */
struct spacemit_drm_private {
	struct drm_device *ddev;
	struct device *dev;
	struct spacemit_crtc *a_crtc;
	struct spacemit_hw_device *hwdev;
	struct cmdlist **cmdlist_groups;
};

extern struct drm_driver spacemit_drm_drv;
extern const struct component_master_ops spacemit_drm_master_ops;

void spacemit_drm_mode_config_init(struct drm_device *drm);
int spacemit_drm_of_component_probe(struct device *dev);

#endif /* _SPACEMIT_DRM_H_ */
