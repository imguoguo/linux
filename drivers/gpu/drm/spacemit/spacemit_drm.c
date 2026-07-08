// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#include <drm/drm_atomic_helper.h>
#include <drm/drm_debugfs.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_gem_shmem_helper.h>
#include <drm/drm_of.h>
#include <drm/drm_fbdev_shmem.h>
#include <linux/component.h>
#include <linux/of_graph.h>

#include "spacemit_drm.h"
#include "spacemit_crtc.h"

#define DRIVER_NAME	"spacemit"
#define DRIVER_DESC	"Spacemit SoCs' DRM Driver"
#define DRIVER_MAJOR	1
#define DRIVER_MINOR	0

static void spacemit_drm_atomic_commit_tail(struct drm_atomic_commit *old_state)
{
	struct drm_device *dev = old_state->dev;

	drm_atomic_helper_commit_modeset_disables(dev, old_state);

	drm_atomic_helper_commit_modeset_enables(dev, old_state);

	drm_atomic_helper_commit_planes(dev, old_state,
					DRM_PLANE_COMMIT_ACTIVE_ONLY);

	drm_atomic_helper_wait_for_flip_done(dev, old_state);

	drm_atomic_helper_commit_hw_done(old_state);

	drm_atomic_helper_cleanup_planes(dev, old_state);
}

static const struct drm_mode_config_helper_funcs spacemit_drm_mode_config_helper = {
	.atomic_commit_tail = spacemit_drm_atomic_commit_tail,
};

/* timgings except vstart, vend and vtotal */
static bool spacemit_drm_mode_match_timings_1(const struct drm_display_mode *mode1,
				   const struct drm_display_mode *mode2)
{
	return mode1->hdisplay == mode2->hdisplay &&
		mode1->hsync_start == mode2->hsync_start &&
		mode1->hsync_end == mode2->hsync_end &&
		mode1->htotal == mode2->htotal &&
		mode1->hskew == mode2->hskew &&
		mode1->vdisplay == mode2->vdisplay &&
		mode1->vscan == mode2->vscan;
}

static bool spacemit_drm_mode_match_timings_2(const struct drm_display_mode *mode1,
				   const struct drm_display_mode *mode2)
{
	return mode1->vsync_start == mode2->vsync_start &&
		mode1->vsync_end == mode2->vsync_end &&
		mode1->vtotal == mode2->vtotal;
}

/* check whether only timing vfp not equal, based on drm_mode_equal */
static bool spacemit_drm_mode_equal(const struct drm_display_mode *mode1,
		const struct drm_display_mode *mode2)
{
	int vfp1, vbp1, vfp2, vbp2;
	bool ret;

	ret = drm_mode_match(mode1, mode2,
			      DRM_MODE_MATCH_CLOCK |
			      DRM_MODE_MATCH_FLAGS |
			      DRM_MODE_MATCH_3D_FLAGS|
			      DRM_MODE_MATCH_ASPECT_RATIO);

	if (ret == false)
		return false;

	if (!spacemit_drm_mode_match_timings_1(mode1, mode2))
		return false;

	vfp1 = mode1->vsync_start - mode1->vdisplay;
	vbp1 = mode1->vtotal - mode1->vsync_end;
	vfp2 = mode2->vsync_start - mode2->vdisplay;
	vbp2 = mode2->vtotal - mode2->vsync_end;
	if (!spacemit_drm_mode_match_timings_2(mode1, mode2)) {
		if ((vbp1 == vbp2) && (vfp1 != vfp2))	/* Just vfp change for fps update */
			return true;
		else
			return false;
	}

	return true;
}

/* based on drm_atomic_helper_check */
static int spacemit_drm_atomic_helper_check(struct drm_device *dev, struct drm_atomic_commit *state)
{
	int ret;
	struct drm_crtc *crtc;
	struct drm_crtc_state *old_crtc_state, *new_crtc_state;
	int i;

	/* original drm api */
	ret = drm_atomic_helper_check(dev, state);

	/* if new crtc mode only update vfp, no need to disable and enable crtc and connector */
	for_each_oldnew_crtc_in_state(state, crtc, old_crtc_state, new_crtc_state, i) {
		if (new_crtc_state->mode_changed) {
			if (spacemit_drm_mode_equal(&old_crtc_state->mode, &new_crtc_state->mode))
				new_crtc_state->mode_changed = false;
		}
	}

	return ret;
}

static const struct drm_mode_config_funcs spacemit_drm_mode_config_funcs = {
	.fb_create = drm_gem_fb_create,
	.atomic_check = spacemit_drm_atomic_helper_check,
	.atomic_commit = drm_atomic_helper_commit,
};

void spacemit_drm_mode_config_init(struct drm_device *drm)
{
	drm_mode_config_init(drm);

	/* HW has no limitation of min width and min height,
	 * even for YUV format, which is limitated in plane check
	 */
	drm->mode_config.min_width = 1;
	drm->mode_config.min_height = 1;
	drm->mode_config.max_width = 4096;
	drm->mode_config.max_height = 4096;

	drm->mode_config.funcs = &spacemit_drm_mode_config_funcs;
	drm->mode_config.helper_private = &spacemit_drm_mode_config_helper;
}

#ifdef CONFIG_DEBUG_FS

#define FRAMEBUFFER_DUMP_PATH "/tmp"
static int spacemit_framebuffer_dump(struct drm_plane *plane)
{
	unsigned int buffer_size = 0;
	int i, j;
	void *mmu_tbl_vaddr = NULL;
	phys_addr_t dpu_buffer_paddr = 0;
	void __iomem *dpu_buffer_vaddr = NULL;
	loff_t pos = 0;
	struct file *filep = NULL;
	struct drm_framebuffer *fb;
	char file_name[128];
	struct spacemit_plane_state *spacemit_pstate = to_spacemit_plane_state(plane->state);

	fb = plane->state->fb;
	if (!fb)
		return -EINVAL;

	for (i = 0; i < fb->format->num_planes; i++) {
		if (fb->obj[i]) {
			char format[5];

			format[0] = (fb->format->format & 0xFF);
			format[1] = ((fb->format->format >> 8) & 0xFF);
			format[2] = ((fb->format->format >> 16) & 0xFF);
			format[3] = ((fb->format->format >> 24) & 0xFF);
			format[4] = '\0';

			if (fb->format->is_yuv) {
				snprintf(file_name, 100, "%s/plane%d_fb%d_%s_planes%d_%dx%d.%s", FRAMEBUFFER_DUMP_PATH, plane->base.id, fb->base.id,
				format, i, fb->width, fb->height, "yuv");
			} else {
				snprintf(file_name, 100, "%s/plane%d_fb%d_%s_planes%d_%dx%d.%s", FRAMEBUFFER_DUMP_PATH, plane->base.id, fb->base.id,
				format, i, fb->width, fb->height, "rgb");
			}

			mmu_tbl_vaddr = spacemit_pstate->mmu_tbl.va;
			buffer_size = plane->state->fb->obj[i]->size >> PAGE_SHIFT;
			filep = filp_open(file_name, O_RDWR | O_APPEND | O_CREAT, 0644);

			if (IS_ERR(filep)) {
				DRM_ERROR("Open file %s failed!\n", file_name);
				return -EINVAL;
			}
			for (j = 0; j < buffer_size; j++) {
				dpu_buffer_paddr = *(volatile u32 __force *)mmu_tbl_vaddr;
				dpu_buffer_paddr = dpu_buffer_paddr << PAGE_SHIFT;
				dpu_buffer_vaddr = phys_to_virt((unsigned long)dpu_buffer_paddr);
				mmu_tbl_vaddr += 4;
				kernel_write(filep, (void *)dpu_buffer_vaddr, PAGE_SIZE, &pos);
			}
			filp_close(filep, NULL);
			filep = NULL;
			DRM_INFO("dump framebuffer: %s\n", file_name);
		}

	}

	return 0;
}

static int spacemit_drm_dump_show(struct seq_file *s, void *data)
{
	struct drm_info_node *node = s->private;
	struct drm_minor *minor = node->minor;
	struct drm_device *drm_dev = minor->dev;
	struct drm_plane *plane;
	struct drm_framebuffer *fb;

	DRM_INFO("%s()\n", __func__);
	mutex_lock(&drm_dev->mode_config.fb_lock);
	drm_for_each_fb(fb, drm_dev) {
		DRM_INFO("framebuffer[%u]\n", fb->base.id);
		drm_for_each_plane(plane, drm_dev) {
			if (plane->state->fb != fb)
				continue;

			spacemit_framebuffer_dump(plane);
		}
	}
	mutex_unlock(&drm_dev->mode_config.fb_lock);

	return 0;
}

static const struct drm_info_list spacemit_debugfs_files[] = {
	{ "dump", spacemit_drm_dump_show, 0 },
};

static void spacemit_drm_debugfs_init(struct drm_minor *minor)
{
	DRM_DEBUG("%s()\n", __func__);
	drm_debugfs_create_files(spacemit_debugfs_files,
						ARRAY_SIZE(spacemit_debugfs_files),
						minor->debugfs_root,
						minor);
}
#endif

DEFINE_DRM_GEM_FOPS(spacemit_drm_fops);

struct drm_driver spacemit_drm_drv = {
	.driver_features = DRIVER_GEM | DRIVER_MODESET |
					DRIVER_ATOMIC | DRIVER_HAVE_IRQ,
	.fops = &spacemit_drm_fops,

	DRM_GEM_SHMEM_DRIVER_OPS,
	DRM_FBDEV_SHMEM_DRIVER_OPS,
#ifdef CONFIG_DEBUG_FS
	.debugfs_init		= spacemit_drm_debugfs_init,
#endif

	.name		= DRIVER_NAME,
	.desc			= DRIVER_DESC,
	.major		= DRIVER_MAJOR,
	.minor		= DRIVER_MINOR,
};

static int compare_of(struct device *dev, void *data)
{
	return dev->of_node == data;
}

int spacemit_drm_of_component_probe(struct device *dev)
{
	struct device_node *ports, *port, *ep;
	struct device_node *remote;
	struct component_match *match = NULL;

	if (!dev->of_node)
		return -EINVAL;

	/*
	 * The DPU is the drm subsystem root. Walk its OF graph topology and
	 * register every available downstream endpoint parent (the DP / eDP
	 * controller) as a component slave.
	 */
	ports = of_get_child_by_name(dev->of_node, "ports");
	if (!ports) {
		dev_err(dev, "missing 'ports' child node\n");
		return -EINVAL;
	}

	for_each_child_of_node(ports, port) {
		if (!of_node_name_eq(port, "port"))
			continue;
		for_each_child_of_node(port, ep) {
			if (!of_node_name_eq(ep, "endpoint"))
				continue;
			remote = of_graph_get_remote_port_parent(ep);
			if (!remote)
				continue;
			if (of_device_is_available(remote))
				drm_of_component_match_add(dev, &match,
							   compare_of, remote);
			of_node_put(remote);
		}
	}
	of_node_put(ports);

	if (!match) {
		dev_err(dev, "no downstream component available\n");
		return -ENODEV;
	}

	return component_master_add_with_match(dev, &spacemit_drm_master_ops,
					       match);
}
