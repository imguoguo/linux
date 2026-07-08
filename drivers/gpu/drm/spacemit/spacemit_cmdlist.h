/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 Spacemit Co., Ltd.
 *
 */

#ifndef _SATURN_FW_CMDLIST_H_
#define _SATURN_FW_CMDLIST_H_

#include <linux/stddef.h>
#include <linux/types.h>
#include <drm/drm_crtc.h>
#include <drm/drm_plane.h>

/*
 * cmdlist v0 : lea, le, lec, lee
 * cmdlist v1 : led
 */
#define PER_CMDLIST_SIZE             4096
#define CMDLIST_ROW_REGS	(3)  //number of regs in each row
#define CMDLIST_CMP_INVALID	(0xff)

/**
 * enum cmdlist_type - what surface owns a cmdlist instance
 * @CMDLIST_PLANE: plane-level cmdlist; carries per-plane RDMA + scaler
 *                 register batches built in spacemit_plane_atomic_update().
 * @CMDLIST_CRTC:  CRTC-level cmdlist; carries the post-pipe / compose-side
 *                 batches submitted on every flush.
 */
enum cmdlist_type {
	CMDLIST_PLANE = 1,
	CMDLIST_CRTC,
};

/**
 * struct cmdlist - One cmdlist submission descriptor
 *
 * A cmdlist batches register writes destined for the DPU into a DMA-backed
 * row sequence that the hardware replays in-order at the next CFG_RDY.
 * One descriptor exists per plane (CMDLIST_PLANE) and per CRTC (CMDLIST_CRTC);
 * the DPU walks them via @next during compose.
 *
 * @nod_len:   Number of register-write rows currently filled.
 * @size:      Capacity of the underlying DMA buffer in bytes.
 * @mode_mask: Bitmask of contributing cmdlist_mode_type_t sources (RDMA, COMP,
 *             DMMU, ...). Used by the v1 hardware to gate which modules' rows
 *             are pulled when CFG_RDY fires.
 * @va:        Kernel virtual address of the DMA buffer.
 * @pa:        DMA address of the buffer, programmed into the cmdlist base
 *             register so the DPU can fetch directly.
 * @next:      Linked list of cmdlists in submission order. The CRTC-level
 *             cmdlist appears last in the chain.
 * @index:     Slot index inside the per-pipeline cmdlist group pool.
 * @type:      Originating surface; see &enum cmdlist_type.
 * @rch_start_cmps_y:   Plane-internal compositor-Y start indicator written
 *                      by spacemit_plane_atomic_update() as a sanity sentinel.
 * @cmdlist_ch_y_other: Companion sentinel kept alongside @rch_start_cmps_y.
 */
struct cmdlist {
	u16 nod_len;        //the number of cmdlist row.
	u32 size;
	u32 mode_mask;		//for cmdlist_v1
	void *va;
	dma_addr_t pa;
	struct cmdlist *next;
	u8 index;	//cmdlist index
	u8 type;
	u32 rch_start_cmps_y;
	u32 cmdlist_ch_y_other;
};

typedef enum {
	CMDLIST_MOD_RDMA = BIT(0),
	CMDLIST_MOD_LP = BIT(1),
	CMDLIST_MOD_SCL = BIT(2),
	CMDLIST_MOD_COMP = BIT(3),
	CMDLIST_MOD_WB0 = BIT(4),
	CMDLIST_MOD_WB1 = BIT(5),
	CMDLIST_MOD_DMMU = BIT(6),
} cmdlist_mode_type_t;

/* reg0: 0xf reg1: 0xf0 reg2: 0xf00*/
#define CMDLIST_REG_STROBE(index)	(0xf << index * 4)

#define CMDLIST_ADDRL_ALIGN_BITS               (4) //From cmdlist_reg_0[] in CMDLIST_REG
#define CMDLIST_ADDRL_ALIGN_MASK               ((u32)(~(BIT(CMDLIST_ADDRL_ALIGN_BITS) - 1)))
#define CONFIG_DRM_SPACEMIT_CMDLIST 1

#ifdef CONFIG_DRM_SPACEMIT_CMDLIST
#define alloc_cmdlist_regs(module_name) \
({ \
	struct cmdlist_regs *cl = kzalloc(sizeof(struct cmdlist_regs), GFP_KERNEL); \
	if (cl == NULL) \
		DRM_ERROR("Failed to allocate struct cmdlist_regs!\n"); \
	cl->size = sizeof(module_name) / sizeof(INT32); \
	cl->module = kzalloc(sizeof(module_name), GFP_KERNEL); \
	if (cl->module == NULL) \
		DRM_ERROR("Failed to allocate module regs!\n"); \
	cl->flags = kzalloc(cl->size, GFP_KERNEL); \
	if (cl->flags == NULL) \
		DRM_ERROR("Failed to allocate module flags!\n"); \
	cl; \
})

#define free_cmdlist_regs(cl_p) \
{ \
	struct cmdlist_regs *cl = (struct cmdlist_regs *)cl_p; \
	if (unlikely(cl == NULL)) \
		DRM_ERROR("NULL cmdlist_regs pointer!\n"); \
	else { \
		kfree(cl->module); \
		kfree(cl->flags); \
		kfree(cl); \
	} \
}

#define cmdlist_write_reg(hwdev, module_name, module_base, field, data, cl_regs, offset) \
{ \
	struct cmdlist_regs *cl_p = (struct cmdlist_regs *)cl_regs; \
	volatile module_name *module = (module_name *)(cl_p->module); \
	if (cl_p->flags[offset] == 0) { \
		volatile module_name *module_hw = ((volatile module_name *)(module_base + \
						  ((struct spacemit_hw_device *)hwdev)->base)); \
		module->value32[offset] = module_hw->value32[offset]; \
		cl_p->base = module_base; \
		cl_p->flags[offset] = 1; \
	} \
	module->field = data; \
}
#else
#define alloc_cmdlist_regs(module_name) NULL
#define free_cmdlist_regs(cl_p) {}
#define cmdlist_write_reg(hwdev, module_name, module_base, field, data, cl_regs, offset) {}
#endif	/* CONFIG_DRM_SPACEMIT_CMDLIST */

struct cmdlist_regs {
	u32 base;
	u16 size;
	void *module;
	u8 *flags;
};

void cmdlist_regs_packing(struct cmdlist *cl, cmdlist_mode_type_t mod, \
			  struct cmdlist_regs *cl_regs);
void cmdlist_sort_by_group(struct drm_crtc *crtc);
void cmdlist_atomic_commit(struct drm_crtc *crtc,
			   struct drm_crtc_state *old_state);
void print_row(u32 *row);
struct spacemit_plane_state *cl_to_spacemit_pstate(const struct cmdlist *cl);
void crtc_cmdlist_sort_by_group(void *a_crtc);
void crtc_cmdlist_atomic_commit(void *a_crtc);
struct spacemit_crtc_state *cl_to_spacemit_cstate(const struct cmdlist *cl);
struct cmdlist *plane_to_cl(struct drm_plane *plane);
struct cmdlist *crtc_to_cl(struct drm_crtc *crtc);

#endif /* _SATURN_FW_CMDLIST_H_ */
