/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (C) 2025 SpacemiT Co., Ltd.
 *
 * Saturn DPU register field map: word offset and mask per field, consumed by
 * the dpu_write()/dpu_read() accessors.
 */

#ifndef SATURN_FIELDS_H
#define SATURN_FIELDS_H

#include <linux/bits.h>

/* CMDLIST_REG (cmdlist_top.h): 128 x 32-bit registers */
#define CMDLIST_REG__NWORDS	128
#define CMDLIST_REG__cmdlist_reg_0__cmdlist_ch_start_addrl_WORD(i)	(0 + (i))
#define CMDLIST_REG__cmdlist_reg_0__cmdlist_ch_start_addrl_MASK	GENMASK(31, 4)
#define CMDLIST_REG__cmdlist_reg_16__cmdlist_ch_start_addrh_WORD(i)	(16 + (i))
#define CMDLIST_REG__cmdlist_reg_16__cmdlist_ch_start_addrh_MASK	GENMASK(5, 0)
#define CMDLIST_REG__cmdlist_reg_32__cmdlist_ch_secu_en_WORD(i)	(32 + (i))
#define CMDLIST_REG__cmdlist_reg_32__cmdlist_ch_secu_en_MASK	GENMASK(0, 0)
#define CMDLIST_REG__cmdlist_reg_32__cmdlist_ch_user_id_WORD(i)	(32 + (i))
#define CMDLIST_REG__cmdlist_reg_32__cmdlist_ch_user_id_MASK	GENMASK(4, 1)
#define CMDLIST_REG__secu_cfg_icg_override_WORD	45
#define CMDLIST_REG__secu_cfg_icg_override_MASK	GENMASK(0, 0)
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arcache_WORD(i)	(48 + (i))
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arcache_MASK	GENMASK(3, 0)
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arqos_WORD(i)	(48 + (i))
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arqos_MASK	GENMASK(7, 4)
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arregion_WORD(i)	(48 + (i))
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_arregion_MASK	GENMASK(11, 8)
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_y_first_WORD(i)	(48 + (i))
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_y_first_MASK	GENMASK(24, 12)
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_osd_dep_WORD(i)	(48 + (i))
#define CMDLIST_REG__cmdlist_reg_48__cmdlist_ch_osd_dep_MASK	GENMASK(28, 25)
#define CMDLIST_REG__cmdlist_reg_64__cmdlist_ch_cont_addrl_WORD(i)	(64 + (i))
#define CMDLIST_REG__cmdlist_reg_64__cmdlist_ch_cont_addrl_MASK	GENMASK(31, 4)
#define CMDLIST_REG__cmdlist_reg_80__cmdlist_ch_cont_addrh_WORD(i)	(80 + (i))
#define CMDLIST_REG__cmdlist_reg_80__cmdlist_ch_cont_addrh_MASK	GENMASK(5, 0)
#define CMDLIST_REG__cmdlist_burst_len_WORD	96
#define CMDLIST_REG__cmdlist_burst_len_MASK	GENMASK(4, 0)
#define CMDLIST_REG__axi_port_sel_WORD	96
#define CMDLIST_REG__axi_port_sel_MASK	GENMASK(6, 5)
#define CMDLIST_REG__onl_arb_ratio_WORD	96
#define CMDLIST_REG__onl_arb_ratio_MASK	GENMASK(9, 7)
#define CMDLIST_REG__reorder_burst_split_en_WORD	96
#define CMDLIST_REG__reorder_burst_split_en_MASK	GENMASK(10, 10)
#define CMDLIST_REG__reorder_burst_split_mode_WORD	96
#define CMDLIST_REG__reorder_burst_split_mode_MASK	GENMASK(11, 11)
#define CMDLIST_REG__reorder_osd_num_WORD	96
#define CMDLIST_REG__reorder_osd_num_MASK	GENMASK(17, 12)
#define CMDLIST_REG__cmdlist_reg_98__cmdlist_ch_y_other_WORD(i)	(98 + (i))
#define CMDLIST_REG__cmdlist_reg_98__cmdlist_ch_y_other_MASK	GENMASK(12, 0)
#define CMDLIST_REG__cmdlist_clr_timeout_th_WORD	114
#define CMDLIST_REG__cmdlist_clr_timeout_th_MASK	GENMASK(31, 16)
#define CMDLIST_REG__cmdlist_pslverr_int_msk_WORD	115
#define CMDLIST_REG__cmdlist_pslverr_int_msk_MASK	GENMASK(0, 0)
#define CMDLIST_REG__cmdlist_ch_clr_timeout_int_msk_WORD	115
#define CMDLIST_REG__cmdlist_ch_clr_timeout_int_msk_MASK	GENMASK(23, 16)
#define CMDLIST_REG__cmdlist_pslverr_ints_WORD	116
#define CMDLIST_REG__cmdlist_pslverr_ints_MASK	GENMASK(0, 0)
#define CMDLIST_REG__cmdlist_ch_clr_timeout_ints_WORD	116
#define CMDLIST_REG__cmdlist_ch_clr_timeout_ints_MASK	GENMASK(28, 16)
#define CMDLIST_REG__cmdlist_pslverr_int_raw_WORD	117
#define CMDLIST_REG__cmdlist_pslverr_int_raw_MASK	GENMASK(0, 0)
#define CMDLIST_REG__cmdlist_ch_clr_timeout_int_raw_WORD	117
#define CMDLIST_REG__cmdlist_ch_clr_timeout_int_raw_MASK	GENMASK(28, 16)
#define CMDLIST_REG__cmdlist_reg_118__cmdlist_ch_dbg0_WORD(i)	(118 + (i))
#define CMDLIST_REG__cmdlist_reg_118__cmdlist_ch_dbg0_MASK	GENMASK(10, 0)
#define CMDLIST_REG__cmdlist_reg_118__cmdlist_ch_dbg1_WORD(i)	(118 + (i))
#define CMDLIST_REG__cmdlist_reg_118__cmdlist_ch_dbg1_MASK	GENMASK(26, 16)
#define CMDLIST_REG__cmdlist_dbg_WORD	126
#define CMDLIST_REG__cmdlist_dbg_MASK	GENMASK(5, 0)
#define CMDLIST_REG__mem_lp_auto_en_WORD	127
#define CMDLIST_REG__mem_lp_auto_en_MASK	GENMASK(1, 0)

/* CMPS_X_REG (composer_x.h): 146 x 32-bit registers */
#define CMPS_X_REG__NWORDS	146
#define CMPS_X_REG__module_enable_WORD	0
#define CMPS_X_REG__module_enable_MASK	GENMASK(0, 0)
#define CMPS_X_REG__dst_w_WORD	1
#define CMPS_X_REG__dst_w_MASK	GENMASK(15, 0)
#define CMPS_X_REG__dst_h_WORD	1
#define CMPS_X_REG__dst_h_MASK	GENMASK(31, 16)
#define CMPS_X_REG__bg_color_r_WORD	2
#define CMPS_X_REG__bg_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__bg_color_g_WORD	3
#define CMPS_X_REG__bg_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__bg_color_b_WORD	4
#define CMPS_X_REG__bg_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__bg_color_a_WORD	5
#define CMPS_X_REG__bg_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer00_area_left_WORD	8
#define CMPS_X_REG__layer00_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer00_area_right_WORD	8
#define CMPS_X_REG__layer00_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer00_area_top_WORD	9
#define CMPS_X_REG__layer00_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer00_area_bottom_WORD	9
#define CMPS_X_REG__layer00_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer00_solid_color_r_WORD	10
#define CMPS_X_REG__layer00_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer00_solid_color_g_WORD	11
#define CMPS_X_REG__layer00_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer00_solid_color_b_WORD	12
#define CMPS_X_REG__layer00_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer00_solid_color_a_WORD	13
#define CMPS_X_REG__layer00_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer00_rsv00_WORD	13
#define CMPS_X_REG__layer00_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer00_rsv01_WORD	13
#define CMPS_X_REG__layer00_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer00_rsv02_WORD	13
#define CMPS_X_REG__layer00_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer00_rsv03_WORD	13
#define CMPS_X_REG__layer00_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer00_rsv04_WORD	13
#define CMPS_X_REG__layer00_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer00_rsv05_WORD	13
#define CMPS_X_REG__layer00_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer00_blend_mode_WORD	13
#define CMPS_X_REG__layer00_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer00_solid_area_mode_WORD	13
#define CMPS_X_REG__layer00_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer00_spe_alpha_mode_WORD	13
#define CMPS_X_REG__layer00_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer00_colorkey_en_WORD	13
#define CMPS_X_REG__layer00_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer00_en_WORD	14
#define CMPS_X_REG__layer00_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer00_blend_sel_WORD	14
#define CMPS_X_REG__layer00_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer00_solid_en_WORD	14
#define CMPS_X_REG__layer00_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer00_dma_id_WORD	14
#define CMPS_X_REG__layer00_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer00_layer_alpha_WORD	14
#define CMPS_X_REG__layer00_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer00_alpha_ratio_WORD	14
#define CMPS_X_REG__layer00_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer01_area_left_WORD	15
#define CMPS_X_REG__layer01_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer01_area_right_WORD	15
#define CMPS_X_REG__layer01_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer01_area_top_WORD	16
#define CMPS_X_REG__layer01_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer01_area_bottom_WORD	16
#define CMPS_X_REG__layer01_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer01_solid_color_r_WORD	17
#define CMPS_X_REG__layer01_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer01_solid_color_g_WORD	18
#define CMPS_X_REG__layer01_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer01_solid_color_b_WORD	19
#define CMPS_X_REG__layer01_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer01_solid_color_a_WORD	20
#define CMPS_X_REG__layer01_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer01_rsv00_WORD	20
#define CMPS_X_REG__layer01_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer01_rsv01_WORD	20
#define CMPS_X_REG__layer01_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer01_rsv02_WORD	20
#define CMPS_X_REG__layer01_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer01_rsv03_WORD	20
#define CMPS_X_REG__layer01_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer01_rsv04_WORD	20
#define CMPS_X_REG__layer01_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer01_rsv05_WORD	20
#define CMPS_X_REG__layer01_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer01_blend_mode_WORD	20
#define CMPS_X_REG__layer01_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer01_solid_area_mode_WORD	20
#define CMPS_X_REG__layer01_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer01_spe_alpha_mode_WORD	20
#define CMPS_X_REG__layer01_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer01_colorkey_en_WORD	20
#define CMPS_X_REG__layer01_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer01_en_WORD	21
#define CMPS_X_REG__layer01_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer01_blend_sel_WORD	21
#define CMPS_X_REG__layer01_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer01_solid_en_WORD	21
#define CMPS_X_REG__layer01_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer01_dma_id_WORD	21
#define CMPS_X_REG__layer01_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer01_layer_alpha_WORD	21
#define CMPS_X_REG__layer01_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer01_alpha_ratio_WORD	21
#define CMPS_X_REG__layer01_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer02_area_left_WORD	22
#define CMPS_X_REG__layer02_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer02_area_right_WORD	22
#define CMPS_X_REG__layer02_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer02_area_top_WORD	23
#define CMPS_X_REG__layer02_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer02_area_bottom_WORD	23
#define CMPS_X_REG__layer02_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer02_solid_color_r_WORD	24
#define CMPS_X_REG__layer02_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer02_solid_color_g_WORD	25
#define CMPS_X_REG__layer02_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer02_solid_color_b_WORD	26
#define CMPS_X_REG__layer02_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer02_solid_color_a_WORD	27
#define CMPS_X_REG__layer02_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer02_rsv00_WORD	27
#define CMPS_X_REG__layer02_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer02_rsv01_WORD	27
#define CMPS_X_REG__layer02_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer02_rsv02_WORD	27
#define CMPS_X_REG__layer02_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer02_rsv03_WORD	27
#define CMPS_X_REG__layer02_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer02_rsv04_WORD	27
#define CMPS_X_REG__layer02_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer02_rsv05_WORD	27
#define CMPS_X_REG__layer02_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer02_blend_mode_WORD	27
#define CMPS_X_REG__layer02_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer02_solid_area_mode_WORD	27
#define CMPS_X_REG__layer02_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer02_spe_alpha_mode_WORD	27
#define CMPS_X_REG__layer02_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer02_colorkey_en_WORD	27
#define CMPS_X_REG__layer02_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer02_en_WORD	28
#define CMPS_X_REG__layer02_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer02_blend_sel_WORD	28
#define CMPS_X_REG__layer02_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer02_solid_en_WORD	28
#define CMPS_X_REG__layer02_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer02_dma_id_WORD	28
#define CMPS_X_REG__layer02_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer02_layer_alpha_WORD	28
#define CMPS_X_REG__layer02_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer02_alpha_ratio_WORD	28
#define CMPS_X_REG__layer02_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer03_area_left_WORD	29
#define CMPS_X_REG__layer03_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer03_area_right_WORD	29
#define CMPS_X_REG__layer03_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer03_area_top_WORD	30
#define CMPS_X_REG__layer03_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer03_area_bottom_WORD	30
#define CMPS_X_REG__layer03_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer03_solid_color_r_WORD	31
#define CMPS_X_REG__layer03_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer03_solid_color_g_WORD	32
#define CMPS_X_REG__layer03_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer03_solid_color_b_WORD	33
#define CMPS_X_REG__layer03_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer03_solid_color_a_WORD	34
#define CMPS_X_REG__layer03_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer03_rsv00_WORD	34
#define CMPS_X_REG__layer03_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer03_rsv01_WORD	34
#define CMPS_X_REG__layer03_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer03_rsv02_WORD	34
#define CMPS_X_REG__layer03_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer03_rsv03_WORD	34
#define CMPS_X_REG__layer03_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer03_rsv04_WORD	34
#define CMPS_X_REG__layer03_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer03_rsv05_WORD	34
#define CMPS_X_REG__layer03_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer03_blend_mode_WORD	34
#define CMPS_X_REG__layer03_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer03_solid_area_mode_WORD	34
#define CMPS_X_REG__layer03_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer03_spe_alpha_mode_WORD	34
#define CMPS_X_REG__layer03_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer03_colorkey_en_WORD	34
#define CMPS_X_REG__layer03_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer03_en_WORD	35
#define CMPS_X_REG__layer03_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer03_blend_sel_WORD	35
#define CMPS_X_REG__layer03_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer03_solid_en_WORD	35
#define CMPS_X_REG__layer03_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer03_dma_id_WORD	35
#define CMPS_X_REG__layer03_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer03_layer_alpha_WORD	35
#define CMPS_X_REG__layer03_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer03_alpha_ratio_WORD	35
#define CMPS_X_REG__layer03_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer04_area_left_WORD	36
#define CMPS_X_REG__layer04_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer04_area_right_WORD	36
#define CMPS_X_REG__layer04_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer04_area_top_WORD	37
#define CMPS_X_REG__layer04_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer04_area_bottom_WORD	37
#define CMPS_X_REG__layer04_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer04_solid_color_r_WORD	38
#define CMPS_X_REG__layer04_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer04_solid_color_g_WORD	39
#define CMPS_X_REG__layer04_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer04_solid_color_b_WORD	40
#define CMPS_X_REG__layer04_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer04_solid_color_a_WORD	41
#define CMPS_X_REG__layer04_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer04_rsv00_WORD	41
#define CMPS_X_REG__layer04_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer04_rsv01_WORD	41
#define CMPS_X_REG__layer04_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer04_rsv02_WORD	41
#define CMPS_X_REG__layer04_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer04_rsv03_WORD	41
#define CMPS_X_REG__layer04_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer04_rsv04_WORD	41
#define CMPS_X_REG__layer04_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer04_rsv05_WORD	41
#define CMPS_X_REG__layer04_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer04_blend_mode_WORD	41
#define CMPS_X_REG__layer04_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer04_solid_area_mode_WORD	41
#define CMPS_X_REG__layer04_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer04_spe_alpha_mode_WORD	41
#define CMPS_X_REG__layer04_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer04_colorkey_en_WORD	41
#define CMPS_X_REG__layer04_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer04_en_WORD	42
#define CMPS_X_REG__layer04_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer04_blend_sel_WORD	42
#define CMPS_X_REG__layer04_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer04_solid_en_WORD	42
#define CMPS_X_REG__layer04_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer04_dma_id_WORD	42
#define CMPS_X_REG__layer04_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer04_layer_alpha_WORD	42
#define CMPS_X_REG__layer04_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer04_alpha_ratio_WORD	42
#define CMPS_X_REG__layer04_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer05_area_left_WORD	43
#define CMPS_X_REG__layer05_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer05_area_right_WORD	43
#define CMPS_X_REG__layer05_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer05_area_top_WORD	44
#define CMPS_X_REG__layer05_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer05_area_bottom_WORD	44
#define CMPS_X_REG__layer05_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer05_solid_color_r_WORD	45
#define CMPS_X_REG__layer05_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer05_solid_color_g_WORD	46
#define CMPS_X_REG__layer05_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer05_solid_color_b_WORD	47
#define CMPS_X_REG__layer05_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer05_solid_color_a_WORD	48
#define CMPS_X_REG__layer05_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer05_rsv00_WORD	48
#define CMPS_X_REG__layer05_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer05_rsv01_WORD	48
#define CMPS_X_REG__layer05_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer05_rsv02_WORD	48
#define CMPS_X_REG__layer05_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer05_rsv03_WORD	48
#define CMPS_X_REG__layer05_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer05_rsv04_WORD	48
#define CMPS_X_REG__layer05_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer05_rsv05_WORD	48
#define CMPS_X_REG__layer05_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer05_blend_mode_WORD	48
#define CMPS_X_REG__layer05_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer05_solid_area_mode_WORD	48
#define CMPS_X_REG__layer05_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer05_spe_alpha_mode_WORD	48
#define CMPS_X_REG__layer05_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer05_colorkey_en_WORD	48
#define CMPS_X_REG__layer05_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer05_en_WORD	49
#define CMPS_X_REG__layer05_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer05_blend_sel_WORD	49
#define CMPS_X_REG__layer05_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer05_solid_en_WORD	49
#define CMPS_X_REG__layer05_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer05_dma_id_WORD	49
#define CMPS_X_REG__layer05_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer05_layer_alpha_WORD	49
#define CMPS_X_REG__layer05_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer05_alpha_ratio_WORD	49
#define CMPS_X_REG__layer05_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer06_area_left_WORD	50
#define CMPS_X_REG__layer06_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer06_area_right_WORD	50
#define CMPS_X_REG__layer06_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer06_area_top_WORD	51
#define CMPS_X_REG__layer06_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer06_area_bottom_WORD	51
#define CMPS_X_REG__layer06_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer06_solid_color_r_WORD	52
#define CMPS_X_REG__layer06_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer06_solid_color_g_WORD	53
#define CMPS_X_REG__layer06_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer06_solid_color_b_WORD	54
#define CMPS_X_REG__layer06_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer06_solid_color_a_WORD	55
#define CMPS_X_REG__layer06_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer06_rsv00_WORD	55
#define CMPS_X_REG__layer06_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer06_rsv01_WORD	55
#define CMPS_X_REG__layer06_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer06_rsv02_WORD	55
#define CMPS_X_REG__layer06_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer06_rsv03_WORD	55
#define CMPS_X_REG__layer06_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer06_rsv04_WORD	55
#define CMPS_X_REG__layer06_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer06_rsv05_WORD	55
#define CMPS_X_REG__layer06_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer06_blend_mode_WORD	55
#define CMPS_X_REG__layer06_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer06_solid_area_mode_WORD	55
#define CMPS_X_REG__layer06_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer06_spe_alpha_mode_WORD	55
#define CMPS_X_REG__layer06_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer06_colorkey_en_WORD	55
#define CMPS_X_REG__layer06_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer06_en_WORD	56
#define CMPS_X_REG__layer06_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer06_blend_sel_WORD	56
#define CMPS_X_REG__layer06_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer06_solid_en_WORD	56
#define CMPS_X_REG__layer06_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer06_dma_id_WORD	56
#define CMPS_X_REG__layer06_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer06_layer_alpha_WORD	56
#define CMPS_X_REG__layer06_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer06_alpha_ratio_WORD	56
#define CMPS_X_REG__layer06_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer07_area_left_WORD	57
#define CMPS_X_REG__layer07_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer07_area_right_WORD	57
#define CMPS_X_REG__layer07_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer07_area_top_WORD	58
#define CMPS_X_REG__layer07_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer07_area_bottom_WORD	58
#define CMPS_X_REG__layer07_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer07_solid_color_r_WORD	59
#define CMPS_X_REG__layer07_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer07_solid_color_g_WORD	60
#define CMPS_X_REG__layer07_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer07_solid_color_b_WORD	61
#define CMPS_X_REG__layer07_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer07_solid_color_a_WORD	62
#define CMPS_X_REG__layer07_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer07_rsv00_WORD	62
#define CMPS_X_REG__layer07_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer07_rsv01_WORD	62
#define CMPS_X_REG__layer07_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer07_rsv02_WORD	62
#define CMPS_X_REG__layer07_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer07_rsv03_WORD	62
#define CMPS_X_REG__layer07_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer07_rsv04_WORD	62
#define CMPS_X_REG__layer07_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer07_rsv05_WORD	62
#define CMPS_X_REG__layer07_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer07_blend_mode_WORD	62
#define CMPS_X_REG__layer07_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer07_solid_area_mode_WORD	62
#define CMPS_X_REG__layer07_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer07_spe_alpha_mode_WORD	62
#define CMPS_X_REG__layer07_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer07_colorkey_en_WORD	62
#define CMPS_X_REG__layer07_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer07_en_WORD	63
#define CMPS_X_REG__layer07_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer07_blend_sel_WORD	63
#define CMPS_X_REG__layer07_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer07_solid_en_WORD	63
#define CMPS_X_REG__layer07_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer07_dma_id_WORD	63
#define CMPS_X_REG__layer07_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer07_layer_alpha_WORD	63
#define CMPS_X_REG__layer07_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer07_alpha_ratio_WORD	63
#define CMPS_X_REG__layer07_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer08_area_left_WORD	64
#define CMPS_X_REG__layer08_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer08_area_right_WORD	64
#define CMPS_X_REG__layer08_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer08_area_top_WORD	65
#define CMPS_X_REG__layer08_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer08_area_bottom_WORD	65
#define CMPS_X_REG__layer08_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer08_solid_color_r_WORD	66
#define CMPS_X_REG__layer08_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer08_solid_color_g_WORD	67
#define CMPS_X_REG__layer08_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer08_solid_color_b_WORD	68
#define CMPS_X_REG__layer08_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer08_solid_color_a_WORD	69
#define CMPS_X_REG__layer08_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer08_rsv00_WORD	69
#define CMPS_X_REG__layer08_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer08_rsv01_WORD	69
#define CMPS_X_REG__layer08_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer08_rsv02_WORD	69
#define CMPS_X_REG__layer08_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer08_rsv03_WORD	69
#define CMPS_X_REG__layer08_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer08_rsv04_WORD	69
#define CMPS_X_REG__layer08_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer08_rsv05_WORD	69
#define CMPS_X_REG__layer08_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer08_blend_mode_WORD	69
#define CMPS_X_REG__layer08_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer08_solid_area_mode_WORD	69
#define CMPS_X_REG__layer08_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer08_spe_alpha_mode_WORD	69
#define CMPS_X_REG__layer08_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer08_colorkey_en_WORD	69
#define CMPS_X_REG__layer08_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer08_en_WORD	70
#define CMPS_X_REG__layer08_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer08_blend_sel_WORD	70
#define CMPS_X_REG__layer08_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer08_solid_en_WORD	70
#define CMPS_X_REG__layer08_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer08_dma_id_WORD	70
#define CMPS_X_REG__layer08_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer08_layer_alpha_WORD	70
#define CMPS_X_REG__layer08_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer08_alpha_ratio_WORD	70
#define CMPS_X_REG__layer08_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer09_area_left_WORD	71
#define CMPS_X_REG__layer09_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer09_area_right_WORD	71
#define CMPS_X_REG__layer09_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer09_area_top_WORD	72
#define CMPS_X_REG__layer09_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer09_area_bottom_WORD	72
#define CMPS_X_REG__layer09_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer09_solid_color_r_WORD	73
#define CMPS_X_REG__layer09_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer09_solid_color_g_WORD	74
#define CMPS_X_REG__layer09_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer09_solid_color_b_WORD	75
#define CMPS_X_REG__layer09_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer09_solid_color_a_WORD	76
#define CMPS_X_REG__layer09_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer09_rsv00_WORD	76
#define CMPS_X_REG__layer09_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer09_rsv01_WORD	76
#define CMPS_X_REG__layer09_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer09_rsv02_WORD	76
#define CMPS_X_REG__layer09_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer09_rsv03_WORD	76
#define CMPS_X_REG__layer09_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer09_rsv04_WORD	76
#define CMPS_X_REG__layer09_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer09_rsv05_WORD	76
#define CMPS_X_REG__layer09_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer09_blend_mode_WORD	76
#define CMPS_X_REG__layer09_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer09_solid_area_mode_WORD	76
#define CMPS_X_REG__layer09_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer09_spe_alpha_mode_WORD	76
#define CMPS_X_REG__layer09_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer09_colorkey_en_WORD	76
#define CMPS_X_REG__layer09_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer09_en_WORD	77
#define CMPS_X_REG__layer09_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer09_blend_sel_WORD	77
#define CMPS_X_REG__layer09_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer09_solid_en_WORD	77
#define CMPS_X_REG__layer09_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer09_dma_id_WORD	77
#define CMPS_X_REG__layer09_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer09_layer_alpha_WORD	77
#define CMPS_X_REG__layer09_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer09_alpha_ratio_WORD	77
#define CMPS_X_REG__layer09_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer10_area_left_WORD	78
#define CMPS_X_REG__layer10_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer10_area_right_WORD	78
#define CMPS_X_REG__layer10_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer10_area_top_WORD	79
#define CMPS_X_REG__layer10_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer10_area_bottom_WORD	79
#define CMPS_X_REG__layer10_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer10_solid_color_r_WORD	80
#define CMPS_X_REG__layer10_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer10_solid_color_g_WORD	81
#define CMPS_X_REG__layer10_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer10_solid_color_b_WORD	82
#define CMPS_X_REG__layer10_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer10_solid_color_a_WORD	83
#define CMPS_X_REG__layer10_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer10_rsv00_WORD	83
#define CMPS_X_REG__layer10_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer10_rsv01_WORD	83
#define CMPS_X_REG__layer10_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer10_rsv02_WORD	83
#define CMPS_X_REG__layer10_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer10_rsv03_WORD	83
#define CMPS_X_REG__layer10_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer10_rsv04_WORD	83
#define CMPS_X_REG__layer10_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer10_rsv05_WORD	83
#define CMPS_X_REG__layer10_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer10_blend_mode_WORD	83
#define CMPS_X_REG__layer10_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer10_solid_area_mode_WORD	83
#define CMPS_X_REG__layer10_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer10_spe_alpha_mode_WORD	83
#define CMPS_X_REG__layer10_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer10_colorkey_en_WORD	83
#define CMPS_X_REG__layer10_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer10_en_WORD	84
#define CMPS_X_REG__layer10_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer10_blend_sel_WORD	84
#define CMPS_X_REG__layer10_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer10_solid_en_WORD	84
#define CMPS_X_REG__layer10_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer10_dma_id_WORD	84
#define CMPS_X_REG__layer10_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer10_layer_alpha_WORD	84
#define CMPS_X_REG__layer10_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer10_alpha_ratio_WORD	84
#define CMPS_X_REG__layer10_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer11_area_left_WORD	85
#define CMPS_X_REG__layer11_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer11_area_right_WORD	85
#define CMPS_X_REG__layer11_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer11_area_top_WORD	86
#define CMPS_X_REG__layer11_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer11_area_bottom_WORD	86
#define CMPS_X_REG__layer11_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer11_solid_color_r_WORD	87
#define CMPS_X_REG__layer11_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer11_solid_color_g_WORD	88
#define CMPS_X_REG__layer11_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer11_solid_color_b_WORD	89
#define CMPS_X_REG__layer11_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer11_solid_color_a_WORD	90
#define CMPS_X_REG__layer11_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer11_rsv00_WORD	90
#define CMPS_X_REG__layer11_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer11_rsv01_WORD	90
#define CMPS_X_REG__layer11_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer11_rsv02_WORD	90
#define CMPS_X_REG__layer11_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer11_rsv03_WORD	90
#define CMPS_X_REG__layer11_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer11_rsv04_WORD	90
#define CMPS_X_REG__layer11_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer11_rsv05_WORD	90
#define CMPS_X_REG__layer11_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer11_blend_mode_WORD	90
#define CMPS_X_REG__layer11_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer11_solid_area_mode_WORD	90
#define CMPS_X_REG__layer11_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer11_spe_alpha_mode_WORD	90
#define CMPS_X_REG__layer11_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer11_colorkey_en_WORD	90
#define CMPS_X_REG__layer11_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer11_en_WORD	91
#define CMPS_X_REG__layer11_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer11_blend_sel_WORD	91
#define CMPS_X_REG__layer11_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer11_solid_en_WORD	91
#define CMPS_X_REG__layer11_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer11_dma_id_WORD	91
#define CMPS_X_REG__layer11_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer11_layer_alpha_WORD	91
#define CMPS_X_REG__layer11_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer11_alpha_ratio_WORD	91
#define CMPS_X_REG__layer11_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer12_area_left_WORD	92
#define CMPS_X_REG__layer12_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer12_area_right_WORD	92
#define CMPS_X_REG__layer12_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer12_area_top_WORD	93
#define CMPS_X_REG__layer12_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer12_area_bottom_WORD	93
#define CMPS_X_REG__layer12_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer12_solid_color_r_WORD	94
#define CMPS_X_REG__layer12_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer12_solid_color_g_WORD	95
#define CMPS_X_REG__layer12_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer12_solid_color_b_WORD	96
#define CMPS_X_REG__layer12_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer12_solid_color_a_WORD	97
#define CMPS_X_REG__layer12_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer12_rsv00_WORD	97
#define CMPS_X_REG__layer12_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer12_rsv01_WORD	97
#define CMPS_X_REG__layer12_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer12_rsv02_WORD	97
#define CMPS_X_REG__layer12_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer12_rsv03_WORD	97
#define CMPS_X_REG__layer12_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer12_rsv04_WORD	97
#define CMPS_X_REG__layer12_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer12_rsv05_WORD	97
#define CMPS_X_REG__layer12_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer12_blend_mode_WORD	97
#define CMPS_X_REG__layer12_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer12_solid_area_mode_WORD	97
#define CMPS_X_REG__layer12_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer12_spe_alpha_mode_WORD	97
#define CMPS_X_REG__layer12_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer12_colorkey_en_WORD	97
#define CMPS_X_REG__layer12_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer12_en_WORD	98
#define CMPS_X_REG__layer12_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer12_blend_sel_WORD	98
#define CMPS_X_REG__layer12_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer12_solid_en_WORD	98
#define CMPS_X_REG__layer12_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer12_dma_id_WORD	98
#define CMPS_X_REG__layer12_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer12_layer_alpha_WORD	98
#define CMPS_X_REG__layer12_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer12_alpha_ratio_WORD	98
#define CMPS_X_REG__layer12_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer13_area_left_WORD	99
#define CMPS_X_REG__layer13_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer13_area_right_WORD	99
#define CMPS_X_REG__layer13_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer13_area_top_WORD	100
#define CMPS_X_REG__layer13_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer13_area_bottom_WORD	100
#define CMPS_X_REG__layer13_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer13_solid_color_r_WORD	101
#define CMPS_X_REG__layer13_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer13_solid_color_g_WORD	102
#define CMPS_X_REG__layer13_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer13_solid_color_b_WORD	103
#define CMPS_X_REG__layer13_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer13_solid_color_a_WORD	104
#define CMPS_X_REG__layer13_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer13_rsv00_WORD	104
#define CMPS_X_REG__layer13_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer13_rsv01_WORD	104
#define CMPS_X_REG__layer13_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer13_rsv02_WORD	104
#define CMPS_X_REG__layer13_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer13_rsv03_WORD	104
#define CMPS_X_REG__layer13_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer13_rsv04_WORD	104
#define CMPS_X_REG__layer13_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer13_rsv05_WORD	104
#define CMPS_X_REG__layer13_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer13_blend_mode_WORD	104
#define CMPS_X_REG__layer13_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer13_solid_area_mode_WORD	104
#define CMPS_X_REG__layer13_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer13_spe_alpha_mode_WORD	104
#define CMPS_X_REG__layer13_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer13_colorkey_en_WORD	104
#define CMPS_X_REG__layer13_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer13_en_WORD	105
#define CMPS_X_REG__layer13_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer13_blend_sel_WORD	105
#define CMPS_X_REG__layer13_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer13_solid_en_WORD	105
#define CMPS_X_REG__layer13_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer13_dma_id_WORD	105
#define CMPS_X_REG__layer13_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer13_layer_alpha_WORD	105
#define CMPS_X_REG__layer13_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer13_alpha_ratio_WORD	105
#define CMPS_X_REG__layer13_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer14_area_left_WORD	106
#define CMPS_X_REG__layer14_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer14_area_right_WORD	106
#define CMPS_X_REG__layer14_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer14_area_top_WORD	107
#define CMPS_X_REG__layer14_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer14_area_bottom_WORD	107
#define CMPS_X_REG__layer14_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer14_solid_color_r_WORD	108
#define CMPS_X_REG__layer14_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer14_solid_color_g_WORD	109
#define CMPS_X_REG__layer14_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer14_solid_color_b_WORD	110
#define CMPS_X_REG__layer14_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer14_solid_color_a_WORD	111
#define CMPS_X_REG__layer14_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer14_rsv00_WORD	111
#define CMPS_X_REG__layer14_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer14_rsv01_WORD	111
#define CMPS_X_REG__layer14_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer14_rsv02_WORD	111
#define CMPS_X_REG__layer14_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer14_rsv03_WORD	111
#define CMPS_X_REG__layer14_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer14_rsv04_WORD	111
#define CMPS_X_REG__layer14_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer14_rsv05_WORD	111
#define CMPS_X_REG__layer14_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer14_blend_mode_WORD	111
#define CMPS_X_REG__layer14_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer14_solid_area_mode_WORD	111
#define CMPS_X_REG__layer14_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer14_spe_alpha_mode_WORD	111
#define CMPS_X_REG__layer14_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer14_colorkey_en_WORD	111
#define CMPS_X_REG__layer14_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer14_en_WORD	112
#define CMPS_X_REG__layer14_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer14_blend_sel_WORD	112
#define CMPS_X_REG__layer14_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer14_solid_en_WORD	112
#define CMPS_X_REG__layer14_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer14_dma_id_WORD	112
#define CMPS_X_REG__layer14_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer14_layer_alpha_WORD	112
#define CMPS_X_REG__layer14_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer14_alpha_ratio_WORD	112
#define CMPS_X_REG__layer14_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer15_area_left_WORD	113
#define CMPS_X_REG__layer15_area_left_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer15_area_right_WORD	113
#define CMPS_X_REG__layer15_area_right_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer15_area_top_WORD	114
#define CMPS_X_REG__layer15_area_top_MASK	GENMASK(15, 0)
#define CMPS_X_REG__layer15_area_bottom_WORD	114
#define CMPS_X_REG__layer15_area_bottom_MASK	GENMASK(31, 16)
#define CMPS_X_REG__layer15_solid_color_r_WORD	115
#define CMPS_X_REG__layer15_solid_color_r_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer15_solid_color_g_WORD	116
#define CMPS_X_REG__layer15_solid_color_g_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer15_solid_color_b_WORD	117
#define CMPS_X_REG__layer15_solid_color_b_MASK	GENMASK(9, 0)
#define CMPS_X_REG__layer15_solid_color_a_WORD	118
#define CMPS_X_REG__layer15_solid_color_a_MASK	GENMASK(7, 0)
#define CMPS_X_REG__layer15_rsv00_WORD	118
#define CMPS_X_REG__layer15_rsv00_MASK	GENMASK(8, 8)
#define CMPS_X_REG__layer15_rsv01_WORD	118
#define CMPS_X_REG__layer15_rsv01_MASK	GENMASK(9, 9)
#define CMPS_X_REG__layer15_rsv02_WORD	118
#define CMPS_X_REG__layer15_rsv02_MASK	GENMASK(10, 10)
#define CMPS_X_REG__layer15_rsv03_WORD	118
#define CMPS_X_REG__layer15_rsv03_MASK	GENMASK(11, 11)
#define CMPS_X_REG__layer15_rsv04_WORD	118
#define CMPS_X_REG__layer15_rsv04_MASK	GENMASK(12, 12)
#define CMPS_X_REG__layer15_rsv05_WORD	118
#define CMPS_X_REG__layer15_rsv05_MASK	GENMASK(13, 13)
#define CMPS_X_REG__layer15_blend_mode_WORD	118
#define CMPS_X_REG__layer15_blend_mode_MASK	GENMASK(15, 14)
#define CMPS_X_REG__layer15_solid_area_mode_WORD	118
#define CMPS_X_REG__layer15_solid_area_mode_MASK	GENMASK(16, 16)
#define CMPS_X_REG__layer15_spe_alpha_mode_WORD	118
#define CMPS_X_REG__layer15_spe_alpha_mode_MASK	GENMASK(17, 17)
#define CMPS_X_REG__layer15_colorkey_en_WORD	118
#define CMPS_X_REG__layer15_colorkey_en_MASK	GENMASK(18, 18)
#define CMPS_X_REG__layer15_en_WORD	119
#define CMPS_X_REG__layer15_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__layer15_blend_sel_WORD	119
#define CMPS_X_REG__layer15_blend_sel_MASK	GENMASK(2, 1)
#define CMPS_X_REG__layer15_solid_en_WORD	119
#define CMPS_X_REG__layer15_solid_en_MASK	GENMASK(3, 3)
#define CMPS_X_REG__layer15_dma_id_WORD	119
#define CMPS_X_REG__layer15_dma_id_MASK	GENMASK(7, 4)
#define CMPS_X_REG__layer15_layer_alpha_WORD	119
#define CMPS_X_REG__layer15_layer_alpha_MASK	GENMASK(15, 8)
#define CMPS_X_REG__layer15_alpha_ratio_WORD	119
#define CMPS_X_REG__layer15_alpha_ratio_MASK	GENMASK(23, 16)
#define CMPS_X_REG__layer_data_not_ready_WORD	120
#define CMPS_X_REG__layer_data_not_ready_MASK	GENMASK(31, 0)
#define CMPS_X_REG__saturn_cmps_reg_121__layer_proc_location_WORD(i)	(121 + (i))
#define CMPS_X_REG__saturn_cmps_reg_121__layer_proc_location_MASK	GENMASK(31, 0)
#define CMPS_X_REG__cmps_proc_location_WORD	137
#define CMPS_X_REG__cmps_proc_location_MASK	GENMASK(31, 0)
#define CMPS_X_REG__cfg_se_WORD	138
#define CMPS_X_REG__cfg_se_MASK	GENMASK(0, 0)
#define CMPS_X_REG__force_update_en_WORD	139
#define CMPS_X_REG__force_update_en_MASK	GENMASK(0, 0)
#define CMPS_X_REG__vsync_update_en_WORD	139
#define CMPS_X_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define CMPS_X_REG__shadow_read_en_WORD	139
#define CMPS_X_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define CMPS_X_REG__force_update_pulse_WORD	140
#define CMPS_X_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define CMPS_X_REG__force_update_en_se_WORD	141
#define CMPS_X_REG__force_update_en_se_MASK	GENMASK(0, 0)
#define CMPS_X_REG__vsync_update_en_se_WORD	141
#define CMPS_X_REG__vsync_update_en_se_MASK	GENMASK(1, 1)
#define CMPS_X_REG__shadow_read_en_se_WORD	141
#define CMPS_X_REG__shadow_read_en_se_MASK	GENMASK(2, 2)
#define CMPS_X_REG__force_update_pulse_se_WORD	142
#define CMPS_X_REG__force_update_pulse_se_MASK	GENMASK(0, 0)
#define CMPS_X_REG__icg_override_WORD	143
#define CMPS_X_REG__icg_override_MASK	GENMASK(0, 0)
#define CMPS_X_REG__trigger_WORD	144
#define CMPS_X_REG__trigger_MASK	GENMASK(0, 0)
#define CMPS_X_REG__trigger2_WORD	145
#define CMPS_X_REG__trigger2_MASK	GENMASK(0, 0)

/* DMA_TOP_REG (rdma_top.h): 25 x 32-bit registers */
#define DMA_TOP_REG__NWORDS	25
#define DMA_TOP_REG__dbg_en_WORD	0
#define DMA_TOP_REG__dbg_en_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__image_rr_ratio_WORD	1
#define DMA_TOP_REG__image_rr_ratio_MASK	GENMASK(7, 0)
#define DMA_TOP_REG__round_robin_mode_WORD	1
#define DMA_TOP_REG__round_robin_mode_MASK	GENMASK(8, 8)
#define DMA_TOP_REG__pixel_num_th_WORD	1
#define DMA_TOP_REG__pixel_num_th_MASK	GENMASK(14, 9)
#define DMA_TOP_REG__rdma_timeout_limit_WORD	2
#define DMA_TOP_REG__rdma_timeout_limit_MASK	GENMASK(15, 0)
#define DMA_TOP_REG__wdma_timeout_limit_WORD	2
#define DMA_TOP_REG__wdma_timeout_limit_MASK	GENMASK(31, 16)
#define DMA_TOP_REG__dmac0_rstn_pwr_WORD	3
#define DMA_TOP_REG__dmac0_rstn_pwr_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__dmac0_rst_req_WORD	3
#define DMA_TOP_REG__dmac0_rst_req_MASK	GENMASK(1, 1)
#define DMA_TOP_REG__dmac0_burst_length_WORD	3
#define DMA_TOP_REG__dmac0_burst_length_MASK	GENMASK(4, 2)
#define DMA_TOP_REG__dmac0_arcache_WORD	3
#define DMA_TOP_REG__dmac0_arcache_MASK	GENMASK(11, 8)
#define DMA_TOP_REG__dmac0_awcache_WORD	3
#define DMA_TOP_REG__dmac0_awcache_MASK	GENMASK(15, 12)
#define DMA_TOP_REG__dmac0_arregion_WORD	3
#define DMA_TOP_REG__dmac0_arregion_MASK	GENMASK(19, 16)
#define DMA_TOP_REG__dmac0_awregion_WORD	3
#define DMA_TOP_REG__dmac0_awregion_MASK	GENMASK(23, 20)
#define DMA_TOP_REG__dmac1_rstn_pwr_WORD	4
#define DMA_TOP_REG__dmac1_rstn_pwr_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__dmac1_rst_req_WORD	4
#define DMA_TOP_REG__dmac1_rst_req_MASK	GENMASK(1, 1)
#define DMA_TOP_REG__dmac1_burst_length_WORD	4
#define DMA_TOP_REG__dmac1_burst_length_MASK	GENMASK(4, 2)
#define DMA_TOP_REG__dmac1_arcache_WORD	4
#define DMA_TOP_REG__dmac1_arcache_MASK	GENMASK(11, 8)
#define DMA_TOP_REG__dmac1_awcache_WORD	4
#define DMA_TOP_REG__dmac1_awcache_MASK	GENMASK(15, 12)
#define DMA_TOP_REG__dmac1_arregion_WORD	4
#define DMA_TOP_REG__dmac1_arregion_MASK	GENMASK(19, 16)
#define DMA_TOP_REG__dmac1_awregion_WORD	4
#define DMA_TOP_REG__dmac1_awregion_MASK	GENMASK(23, 20)
#define DMA_TOP_REG__dmac2_rstn_pwr_WORD	5
#define DMA_TOP_REG__dmac2_rstn_pwr_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__dmac2_rst_req_WORD	5
#define DMA_TOP_REG__dmac2_rst_req_MASK	GENMASK(1, 1)
#define DMA_TOP_REG__dmac2_burst_length_WORD	5
#define DMA_TOP_REG__dmac2_burst_length_MASK	GENMASK(4, 2)
#define DMA_TOP_REG__dmac2_arcache_WORD	5
#define DMA_TOP_REG__dmac2_arcache_MASK	GENMASK(11, 8)
#define DMA_TOP_REG__dmac2_awcache_WORD	5
#define DMA_TOP_REG__dmac2_awcache_MASK	GENMASK(15, 12)
#define DMA_TOP_REG__dmac2_arregion_WORD	5
#define DMA_TOP_REG__dmac2_arregion_MASK	GENMASK(19, 16)
#define DMA_TOP_REG__dmac2_awregion_WORD	5
#define DMA_TOP_REG__dmac2_awregion_MASK	GENMASK(23, 20)
#define DMA_TOP_REG__dmac3_rstn_pwr_WORD	6
#define DMA_TOP_REG__dmac3_rstn_pwr_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__dmac3_rst_req_WORD	6
#define DMA_TOP_REG__dmac3_rst_req_MASK	GENMASK(1, 1)
#define DMA_TOP_REG__dmac3_burst_length_WORD	6
#define DMA_TOP_REG__dmac3_burst_length_MASK	GENMASK(4, 2)
#define DMA_TOP_REG__dmac3_arcache_WORD	6
#define DMA_TOP_REG__dmac3_arcache_MASK	GENMASK(11, 8)
#define DMA_TOP_REG__dmac3_awcache_WORD	6
#define DMA_TOP_REG__dmac3_awcache_MASK	GENMASK(15, 12)
#define DMA_TOP_REG__dmac3_arregion_WORD	6
#define DMA_TOP_REG__dmac3_arregion_MASK	GENMASK(19, 16)
#define DMA_TOP_REG__dmac3_awregion_WORD	6
#define DMA_TOP_REG__dmac3_awregion_MASK	GENMASK(23, 20)
#define DMA_TOP_REG__online_rqos_WORD	7
#define DMA_TOP_REG__online_rqos_MASK	GENMASK(3, 0)
#define DMA_TOP_REG__offline_rqos_WORD	7
#define DMA_TOP_REG__offline_rqos_MASK	GENMASK(7, 4)
#define DMA_TOP_REG__online_wqos_WORD	7
#define DMA_TOP_REG__online_wqos_MASK	GENMASK(11, 8)
#define DMA_TOP_REG__offline_wqos_WORD	7
#define DMA_TOP_REG__offline_wqos_MASK	GENMASK(15, 12)
#define DMA_TOP_REG__cmdlist_rqos_WORD	7
#define DMA_TOP_REG__cmdlist_rqos_MASK	GENMASK(19, 16)
#define DMA_TOP_REG__dmac0_rd_oust_num_WORD	8
#define DMA_TOP_REG__dmac0_rd_oust_num_MASK	GENMASK(7, 0)
#define DMA_TOP_REG__dmac0_wr_oust_num_WORD	8
#define DMA_TOP_REG__dmac0_wr_oust_num_MASK	GENMASK(15, 8)
#define DMA_TOP_REG__dmac1_rd_oust_num_WORD	9
#define DMA_TOP_REG__dmac1_rd_oust_num_MASK	GENMASK(7, 0)
#define DMA_TOP_REG__dmac1_wr_oust_num_WORD	9
#define DMA_TOP_REG__dmac1_wr_oust_num_MASK	GENMASK(15, 8)
#define DMA_TOP_REG__dmac2_rd_oust_num_WORD	10
#define DMA_TOP_REG__dmac2_rd_oust_num_MASK	GENMASK(7, 0)
#define DMA_TOP_REG__dmac2_wr_oust_num_WORD	10
#define DMA_TOP_REG__dmac2_wr_oust_num_MASK	GENMASK(15, 8)
#define DMA_TOP_REG__dmac3_rd_oust_num_WORD	11
#define DMA_TOP_REG__dmac3_rd_oust_num_MASK	GENMASK(7, 0)
#define DMA_TOP_REG__dmac3_wr_oust_num_WORD	11
#define DMA_TOP_REG__dmac3_wr_oust_num_MASK	GENMASK(15, 8)
#define DMA_TOP_REG__cmdlist_dbg_irq_raw_WORD	12
#define DMA_TOP_REG__cmdlist_dbg_irq_raw_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__cmdlist_dbg_irq_mask_WORD	13
#define DMA_TOP_REG__cmdlist_dbg_irq_mask_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__cmdlist_dbg_irq_status_WORD	14
#define DMA_TOP_REG__cmdlist_dbg_irq_status_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__wb0_dbg_irq_raw_WORD	15
#define DMA_TOP_REG__wb0_dbg_irq_raw_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__wb0_dbg_irq_mask_WORD	16
#define DMA_TOP_REG__wb0_dbg_irq_mask_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__wb0_dbg_irq_status_WORD	17
#define DMA_TOP_REG__wb0_dbg_irq_status_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__arb_debug_info_axi0_WORD	18
#define DMA_TOP_REG__arb_debug_info_axi0_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__arb_debug_info_axi1_WORD	19
#define DMA_TOP_REG__arb_debug_info_axi1_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__arb_debug_info_axi2_WORD	20
#define DMA_TOP_REG__arb_debug_info_axi2_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__arb_debug_info_axi3_WORD	21
#define DMA_TOP_REG__arb_debug_info_axi3_MASK	GENMASK(31, 0)
#define DMA_TOP_REG__icg_override_WORD	22
#define DMA_TOP_REG__icg_override_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__regbank_reset_WORD	23
#define DMA_TOP_REG__regbank_reset_MASK	GENMASK(0, 0)
#define DMA_TOP_REG__regbank_reset2_WORD	24
#define DMA_TOP_REG__regbank_reset2_MASK	GENMASK(0, 0)

/* DPU_CTL_REG (dpu_scene_ctl.h): 15 x 32-bit registers */
#define DPU_CTL_REG__NWORDS	15
#define DPU_CTL_REG__nml_rch_en_WORD	0
#define DPU_CTL_REG__nml_rch_en_MASK	GENMASK(9, 0)
#define DPU_CTL_REG__nml_scl_en_WORD	0
#define DPU_CTL_REG__nml_scl_en_MASK	GENMASK(13, 10)
#define DPU_CTL_REG__nml_wb_en_WORD	0
#define DPU_CTL_REG__nml_wb_en_MASK	GENMASK(17, 16)
#define DPU_CTL_REG__nml_outctl_en_WORD	0
#define DPU_CTL_REG__nml_outctl_en_MASK	GENMASK(18, 18)
#define DPU_CTL_REG__nml_dscw_en_WORD	0
#define DPU_CTL_REG__nml_dscw_en_MASK	GENMASK(20, 19)
#define DPU_CTL_REG__nml_dscr_en_WORD	0
#define DPU_CTL_REG__nml_dscr_en_MASK	GENMASK(22, 21)
#define DPU_CTL_REG__nml_frm_timing_en_WORD	0
#define DPU_CTL_REG__nml_frm_timing_en_MASK	GENMASK(25, 23)
#define DPU_CTL_REG__nml_cmd_updt_en_WORD	1
#define DPU_CTL_REG__nml_cmd_updt_en_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__both_cfg_rdy_WORD	2
#define DPU_CTL_REG__both_cfg_rdy_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__sw_clr_WORD	2
#define DPU_CTL_REG__sw_clr_MASK	GENMASK(1, 1)
#define DPU_CTL_REG__sw_clr_st_WORD	2
#define DPU_CTL_REG__sw_clr_st_MASK	GENMASK(2, 2)
#define DPU_CTL_REG__nml_cfg_rdy_WORD	2
#define DPU_CTL_REG__nml_cfg_rdy_MASK	GENMASK(3, 3)
#define DPU_CTL_REG__video_mod_WORD	3
#define DPU_CTL_REG__video_mod_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__dbg_mod_WORD	3
#define DPU_CTL_REG__dbg_mod_MASK	GENMASK(1, 1)
#define DPU_CTL_REG__timing_inter0_WORD	3
#define DPU_CTL_REG__timing_inter0_MASK	GENMASK(7, 2)
#define DPU_CTL_REG__timing_inter1_WORD	3
#define DPU_CTL_REG__timing_inter1_MASK	GENMASK(13, 8)
#define DPU_CTL_REG__sw_start_WORD	4
#define DPU_CTL_REG__sw_start_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__dbg_clr_WORD	4
#define DPU_CTL_REG__dbg_clr_MASK	GENMASK(1, 1)
#define DPU_CTL_REG__reg_nml_value_rst_WORD	5
#define DPU_CTL_REG__reg_nml_value_rst_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__nml_init_en_WORD	6
#define DPU_CTL_REG__nml_init_en_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__nml_rch_vrt_reuse_WORD	7
#define DPU_CTL_REG__nml_rch_vrt_reuse_MASK	GENMASK(9, 0)
#define DPU_CTL_REG__cmd_auto_refresh_en_WORD	7
#define DPU_CTL_REG__cmd_auto_refresh_en_MASK	GENMASK(10, 10)
#define DPU_CTL_REG__secu_rch_en_WORD	8
#define DPU_CTL_REG__secu_rch_en_MASK	GENMASK(9, 0)
#define DPU_CTL_REG__secu_rch_vrt_reuse_WORD	8
#define DPU_CTL_REG__secu_rch_vrt_reuse_MASK	GENMASK(19, 10)
#define DPU_CTL_REG__secu_scl_en_WORD	8
#define DPU_CTL_REG__secu_scl_en_MASK	GENMASK(23, 20)
#define DPU_CTL_REG__secu_cmd_updt_en_WORD	9
#define DPU_CTL_REG__secu_cmd_updt_en_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__secu_cfg_icg_override_WORD	9
#define DPU_CTL_REG__secu_cfg_icg_override_MASK	GENMASK(1, 1)
#define DPU_CTL_REG__secu_cfg_rdy_WORD	10
#define DPU_CTL_REG__secu_cfg_rdy_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__secu_dscw_disable_WORD	11
#define DPU_CTL_REG__secu_dscw_disable_MASK	GENMASK(1, 0)
#define DPU_CTL_REG__secu_wb_disable_WORD	11
#define DPU_CTL_REG__secu_wb_disable_MASK	GENMASK(3, 2)
#define DPU_CTL_REG__reg_secu_value_rst_WORD	12
#define DPU_CTL_REG__reg_secu_value_rst_MASK	GENMASK(0, 0)
#define DPU_CTL_REG__scene_ctl_dbg0_WORD	13
#define DPU_CTL_REG__scene_ctl_dbg0_MASK	GENMASK(28, 0)
#define DPU_CTL_REG__scene_ctl_dbg1_WORD	14
#define DPU_CTL_REG__scene_ctl_dbg1_MASK	GENMASK(31, 0)

/* DPU_CTL_TOP_REG (dpu_ctl_top.h): 53 x 32-bit registers */
#define DPU_CTL_TOP_REG__NWORDS	53
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_0__cmdlist_rch_en_WORD(i)	(0 + (i))
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_0__cmdlist_rch_en_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_10__cmdlist_cmps_other_en_WORD(i)	(10 + (i))
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_10__cmdlist_cmps_other_en_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_10__cmdlist_cmps_top_en_WORD(i)	(10 + (i))
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_10__cmdlist_cmps_top_en_MASK	GENMASK(1, 1)
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_rdy_WORD	13
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_rdy_MASK	GENMASK(9, 0)
#define DPU_CTL_TOP_REG__cmdlist_prepq_cfg_rdy_WORD	13
#define DPU_CTL_TOP_REG__cmdlist_prepq_cfg_rdy_MASK	GENMASK(19, 10)
#define DPU_CTL_TOP_REG__cmdlist_cmps_cfg_rdy_WORD	13
#define DPU_CTL_TOP_REG__cmdlist_cmps_cfg_rdy_MASK	GENMASK(22, 20)
#define DPU_CTL_TOP_REG__cmdlist_wb_cfg_rdy_WORD	13
#define DPU_CTL_TOP_REG__cmdlist_wb_cfg_rdy_MASK	GENMASK(24, 23)
#define DPU_CTL_TOP_REG__wb0_sel_id_WORD	14
#define DPU_CTL_TOP_REG__wb0_sel_id_MASK	GENMASK(4, 0)
#define DPU_CTL_TOP_REG__wb1_sel_id_WORD	15
#define DPU_CTL_TOP_REG__wb1_sel_id_MASK	GENMASK(4, 0)
#define DPU_CTL_TOP_REG__reuse_rdma_act_WORD	16
#define DPU_CTL_TOP_REG__reuse_rdma_act_MASK	GENMASK(9, 0)
#define DPU_CTL_TOP_REG__reuse_prepq_act_WORD	16
#define DPU_CTL_TOP_REG__reuse_prepq_act_MASK	GENMASK(19, 10)
#define DPU_CTL_TOP_REG__reuse_scl_act0_WORD	16
#define DPU_CTL_TOP_REG__reuse_scl_act0_MASK	GENMASK(21, 20)
#define DPU_CTL_TOP_REG__reuse_cmps_act_WORD	16
#define DPU_CTL_TOP_REG__reuse_cmps_act_MASK	GENMASK(24, 22)
#define DPU_CTL_TOP_REG__reuse_wb_act_WORD	16
#define DPU_CTL_TOP_REG__reuse_wb_act_MASK	GENMASK(26, 25)
#define DPU_CTL_TOP_REG__reuse_scl_act1_WORD	16
#define DPU_CTL_TOP_REG__reuse_scl_act1_MASK	GENMASK(28, 27)
#define DPU_CTL_TOP_REG__rch_conflict_ints_WORD	17
#define DPU_CTL_TOP_REG__rch_conflict_ints_MASK	GENMASK(9, 0)
#define DPU_CTL_TOP_REG__acad_timeout_ints_WORD	17
#define DPU_CTL_TOP_REG__acad_timeout_ints_MASK	GENMASK(10, 10)
#define DPU_CTL_TOP_REG__wb_timeout_ints_WORD	17
#define DPU_CTL_TOP_REG__wb_timeout_ints_MASK	GENMASK(12, 11)
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_ints_WORD	17
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_ints_MASK	GENMASK(22, 13)
#define DPU_CTL_TOP_REG__rch_conflict_int_en_WORD	18
#define DPU_CTL_TOP_REG__rch_conflict_int_en_MASK	GENMASK(9, 0)
#define DPU_CTL_TOP_REG__acad_timeout_int_en_WORD	18
#define DPU_CTL_TOP_REG__acad_timeout_int_en_MASK	GENMASK(10, 10)
#define DPU_CTL_TOP_REG__wb_timeout_int_en_WORD	18
#define DPU_CTL_TOP_REG__wb_timeout_int_en_MASK	GENMASK(12, 11)
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_int_en_WORD	18
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_int_en_MASK	GENMASK(22, 13)
#define DPU_CTL_TOP_REG__rch_conflict_int_raw_WORD	19
#define DPU_CTL_TOP_REG__rch_conflict_int_raw_MASK	GENMASK(9, 0)
#define DPU_CTL_TOP_REG__acad_timeout_int_raw_WORD	19
#define DPU_CTL_TOP_REG__acad_timeout_int_raw_MASK	GENMASK(10, 10)
#define DPU_CTL_TOP_REG__wb_timeout_int_raw_WORD	19
#define DPU_CTL_TOP_REG__wb_timeout_int_raw_MASK	GENMASK(12, 11)
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_int_raw_WORD	19
#define DPU_CTL_TOP_REG__cmdlist_rdma_cfg_timeout_int_raw_MASK	GENMASK(22, 13)
#define DPU_CTL_TOP_REG__wb_sel_secu_WORD	20
#define DPU_CTL_TOP_REG__wb_sel_secu_MASK	GENMASK(1, 0)
#define DPU_CTL_TOP_REG__secu_cfg_icg_override_WORD	20
#define DPU_CTL_TOP_REG__secu_cfg_icg_override_MASK	GENMASK(2, 2)
#define DPU_CTL_TOP_REG__scl_sel_secu_WORD	20
#define DPU_CTL_TOP_REG__scl_sel_secu_MASK	GENMASK(4, 3)
#define DPU_CTL_TOP_REG__pslverr_addr_WORD	21
#define DPU_CTL_TOP_REG__pslverr_addr_MASK	GENMASK(7, 0)
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_22__cmdlist_ch_sw_event_WORD(i)	(22 + (i))
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_22__cmdlist_ch_sw_event_MASK	GENMASK(3, 0)
#define DPU_CTL_TOP_REG__ctl_rd_shadow_WORD	35
#define DPU_CTL_TOP_REG__ctl_rd_shadow_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_36__rch_start_cmps_y_WORD(i)	(36 + (i))
#define DPU_CTL_TOP_REG__dpu_ctl_top_reg_36__rch_start_cmps_y_MASK	GENMASK(15, 0)
#define DPU_CTL_TOP_REG__wb0_slice_cnt_WORD	46
#define DPU_CTL_TOP_REG__wb0_slice_cnt_MASK	GENMASK(4, 0)
#define DPU_CTL_TOP_REG__wb1_slice_cnt_WORD	46
#define DPU_CTL_TOP_REG__wb1_slice_cnt_MASK	GENMASK(9, 5)
#define DPU_CTL_TOP_REG__wb_busy_WORD	46
#define DPU_CTL_TOP_REG__wb_busy_MASK	GENMASK(11, 10)
#define DPU_CTL_TOP_REG__acad_busy_WORD	46
#define DPU_CTL_TOP_REG__acad_busy_MASK	GENMASK(13, 12)
#define DPU_CTL_TOP_REG__dscw_busy_WORD	46
#define DPU_CTL_TOP_REG__dscw_busy_MASK	GENMASK(15, 14)
#define DPU_CTL_TOP_REG__nml_scl0_reuse_en_WORD	49
#define DPU_CTL_TOP_REG__nml_scl0_reuse_en_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__nml_scl1_reuse_en_WORD	50
#define DPU_CTL_TOP_REG__nml_scl1_reuse_en_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__nml_scl2_reuse_en_WORD	51
#define DPU_CTL_TOP_REG__nml_scl2_reuse_en_MASK	GENMASK(0, 0)
#define DPU_CTL_TOP_REG__nml_scl3_reuse_en_WORD	52
#define DPU_CTL_TOP_REG__nml_scl3_reuse_en_MASK	GENMASK(0, 0)

/* DPU_INTP_REG (dpu_int.h): 50 x 32-bit registers */
#define DPU_INTP_REG__NWORDS	50
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_int_msk_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_frm_timing_eof_int_msk_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_int_msk_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_int_msk_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_int_msk_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_wb_frm_done_int_msk_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_int_msk_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_wb_ovflow_int_msk_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_int_msk_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_wb_slice_done_int_msk_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_int_msk_WORD	3
#define DPU_INTP_REG__b_video_mode_enter_idle_int_msk_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_int_msk_WORD	3
#define DPU_INTP_REG__b_acad_eof_int_msk_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_te_int_msk_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_vsync_updt_int_msk_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_int_msk_WORD	3
#define DPU_INTP_REG__b_cmb_vsync_done_free_int_msk_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_int_msk_WORD	4
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_int_msk_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_int_msk_WORD	4
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_int_msk_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_int_msk_WORD	4
#define DPU_INTP_REG__b_cmb_cfg_eof_free_int_msk_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_int_msk_WORD	4
#define DPU_INTP_REG__b_cmb_eof_free_int_msk_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_int_msk_WORD	4
#define DPU_INTP_REG__b_cmb_sof_free_int_msk_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_int_msk_WORD	5
#define DPU_INTP_REG__b_cmb_dma_dbg_int_msk_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_int_msk_WORD	5
#define DPU_INTP_REG__b_cmb_outctl_dbg_int_msk_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_int_msk_WORD	5
#define DPU_INTP_REG__b_cmb_ctl_dbg_int_msk_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_int_msk_WORD	5
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_int_msk_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_int_msk_WORD	6
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_int_msk_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_offl0_wb_frm_done_int_msk_WORD	6
#define DPU_INTP_REG__b_offl0_wb_frm_done_int_msk_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_offl0_wb_slice_done_int_msk_WORD	6
#define DPU_INTP_REG__b_offl0_wb_slice_done_int_msk_MASK	GENMASK(3, 3)
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_int_msk_WORD	6
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_int_msk_MASK	GENMASK(12, 5)
#define DPU_INTP_REG__b_offl0_cmdlist_ch_enter_pend_int_msk_WORD	7
#define DPU_INTP_REG__b_offl0_cmdlist_ch_enter_pend_int_msk_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_offl0_arb_dec_pslverr_int_msk_WORD	7
#define DPU_INTP_REG__b_offl0_arb_dec_pslverr_int_msk_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_int_msk_WORD	8
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_int_msk_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_int_msk_WORD	8
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_int_msk_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_int_msk_WORD	8
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_int_msk_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_ints_WORD	12
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_ints_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_ints_WORD	12
#define DPU_INTP_REG__b_cmb_frm_timing_eof_ints_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_ints_WORD	12
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_ints_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_ints_WORD	12
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_ints_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_ints_WORD	12
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_ints_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_ints_WORD	12
#define DPU_INTP_REG__b_cmb_wb_frm_done_ints_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_ints_WORD	12
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_ints_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_ints_WORD	12
#define DPU_INTP_REG__b_cmb_wb_ovflow_ints_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_ints_WORD	12
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_ints_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_ints_WORD	12
#define DPU_INTP_REG__b_cmb_wb_slice_done_ints_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_ints_WORD	12
#define DPU_INTP_REG__b_video_mode_enter_idle_ints_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_ints_WORD	12
#define DPU_INTP_REG__b_acad_eof_ints_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_ints_WORD	12
#define DPU_INTP_REG__b_cmb_te_ints_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_ints_WORD	12
#define DPU_INTP_REG__b_cmb_vsync_updt_ints_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_ints_WORD	12
#define DPU_INTP_REG__b_cmb_vsync_done_free_ints_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_ints_WORD	13
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_ints_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_ints_WORD	13
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_ints_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_ints_WORD	13
#define DPU_INTP_REG__b_cmb_cfg_eof_free_ints_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_ints_WORD	13
#define DPU_INTP_REG__b_cmb_eof_free_ints_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_ints_WORD	13
#define DPU_INTP_REG__b_cmb_sof_free_ints_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_ints_WORD	14
#define DPU_INTP_REG__b_cmb_dma_dbg_ints_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_ints_WORD	14
#define DPU_INTP_REG__b_cmb_outctl_dbg_ints_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_ints_WORD	14
#define DPU_INTP_REG__b_cmb_ctl_dbg_ints_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_ints_WORD	14
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_ints_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_ints_WORD	15
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_ints_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_offl0_wb_frm_done_ints_WORD	15
#define DPU_INTP_REG__b_offl0_wb_frm_done_ints_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_offl0_wb_slice_done_ints_WORD	15
#define DPU_INTP_REG__b_offl0_wb_slice_done_ints_MASK	GENMASK(3, 3)
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_ints_WORD	15
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_ints_MASK	GENMASK(12, 5)
#define DPU_INTP_REG__b_off0_cmdlist_ch_enter_pend_ints_WORD	16
#define DPU_INTP_REG__b_off0_cmdlist_ch_enter_pend_ints_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_off0_arb_dec_pslverr_ints_WORD	16
#define DPU_INTP_REG__b_off0_arb_dec_pslverr_ints_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_ints_WORD	17
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_ints_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_ints_WORD	17
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_ints_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_ints_WORD	17
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_ints_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_int_raw_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_frm_timing_eof_int_raw_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_int_raw_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_int_raw_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_int_raw_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_wb_frm_done_int_raw_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_int_raw_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_wb_ovflow_int_raw_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_int_raw_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_wb_slice_done_int_raw_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_int_raw_WORD	22
#define DPU_INTP_REG__b_video_mode_enter_idle_int_raw_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_int_raw_WORD	22
#define DPU_INTP_REG__b_acad_eof_int_raw_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_te_int_raw_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_vsync_updt_int_raw_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_int_raw_WORD	22
#define DPU_INTP_REG__b_cmb_vsync_done_free_int_raw_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_int_raw_WORD	23
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_int_raw_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_int_raw_WORD	23
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_int_raw_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_int_raw_WORD	23
#define DPU_INTP_REG__b_cmb_cfg_eof_free_int_raw_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_int_raw_WORD	23
#define DPU_INTP_REG__b_cmb_eof_free_int_raw_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_int_raw_WORD	23
#define DPU_INTP_REG__b_cmb_sof_free_int_raw_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_int_raw_WORD	24
#define DPU_INTP_REG__b_cmb_dma_dbg_int_raw_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_int_raw_WORD	24
#define DPU_INTP_REG__b_cmb_outctl_dbg_int_raw_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_int_raw_WORD	24
#define DPU_INTP_REG__b_cmb_ctl_dbg_int_raw_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_int_raw_WORD	24
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_int_raw_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_cmb_arb_dec_paddr_WORD	25
#define DPU_INTP_REG__b_cmb_arb_dec_paddr_MASK	GENMASK(20, 0)
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_int_raw_WORD	26
#define DPU_INTP_REG__b_offl0_cfg_rdy_clr_int_raw_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_offl0_wb_frm_done_int_raw_WORD	26
#define DPU_INTP_REG__b_offl0_wb_frm_done_int_raw_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_offl0_wb_slice_done_int_raw_WORD	26
#define DPU_INTP_REG__b_offl0_wb_slice_done_int_raw_MASK	GENMASK(3, 3)
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_int_raw_WORD	26
#define DPU_INTP_REG__b_offl0_cmdlist_ch_frm_cfg_done_int_raw_MASK	GENMASK(12, 5)
#define DPU_INTP_REG__b_off0_cmdlist_ch_enter_pend_int_raw_WORD	27
#define DPU_INTP_REG__b_off0_cmdlist_ch_enter_pend_int_raw_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_off0_arb_dec_pslverr_int_raw_WORD	27
#define DPU_INTP_REG__b_off0_arb_dec_pslverr_int_raw_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_int_raw_WORD	28
#define DPU_INTP_REG__b_offl0_nml_dma_dbg_int_raw_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_int_raw_WORD	28
#define DPU_INTP_REG__b_offl0_nml_ctl_dbg_int_raw_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_int_raw_WORD	28
#define DPU_INTP_REG__b_offl0_nml_cmdlist_dbg_int_raw_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_offl0_arb_dec_paddr_WORD	29
#define DPU_INTP_REG__b_offl0_arb_dec_paddr_MASK	GENMASK(20, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_int_msk_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_int_msk_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_int_msk_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_int_msk_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_int_msk_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_int_msk_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_int_msk_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_int_msk_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_int_msk_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_int_msk_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_int_msk_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_acad_eof_secu_int_msk_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_te_secu_int_msk_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_int_msk_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_int_msk_WORD	33
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_int_msk_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_int_msk_WORD	34
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_int_msk_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_int_msk_WORD	34
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_int_msk_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_int_msk_WORD	34
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_int_msk_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_secu_int_msk_WORD	34
#define DPU_INTP_REG__b_cmb_eof_free_secu_int_msk_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_secu_int_msk_WORD	34
#define DPU_INTP_REG__b_cmb_sof_free_secu_int_msk_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_int_msk_WORD	35
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_int_msk_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_int_msk_WORD	35
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_int_msk_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_int_msk_WORD	35
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_int_msk_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_int_msk_WORD	35
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_int_msk_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_ints_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_ints_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_ints_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_ints_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_ints_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_ints_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_ints_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_ints_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_ints_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_ints_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_ints_WORD	39
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_ints_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_secu_ints_WORD	39
#define DPU_INTP_REG__b_acad_eof_secu_ints_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_te_secu_ints_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_ints_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_ints_WORD	39
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_ints_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_ints_WORD	40
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_ints_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_ints_WORD	40
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_ints_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_ints_WORD	40
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_ints_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_secu_ints_WORD	40
#define DPU_INTP_REG__b_cmb_eof_free_secu_ints_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_secu_ints_WORD	40
#define DPU_INTP_REG__b_cmb_sof_free_secu_ints_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_ints_WORD	41
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_ints_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_ints_WORD	41
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_ints_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_ints_WORD	41
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_ints_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_ints_WORD	41
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_ints_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_frm_timing_vsync_secu_int_raw_MASK	GENMASK(0, 0)
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_frm_timing_eof_secu_int_raw_MASK	GENMASK(1, 1)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_eof_secu_int_raw_MASK	GENMASK(3, 2)
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_frm_timing_cfg_line_secu_int_raw_MASK	GENMASK(4, 4)
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_cfg_rdy_clr_secu_int_raw_MASK	GENMASK(5, 5)
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_wb_frm_done_secu_int_raw_MASK	GENMASK(6, 6)
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_frm_timing_unflow_secu_int_raw_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_wb_ovflow_secu_int_raw_MASK	GENMASK(9, 9)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_cmdlist_ch_frm_cfg_done_secu_int_raw_MASK	GENMASK(18, 11)
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_wb_slice_done_secu_int_raw_MASK	GENMASK(24, 24)
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_video_mode_enter_idle_secu_int_raw_MASK	GENMASK(26, 26)
#define DPU_INTP_REG__b_acad_eof_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_acad_eof_secu_int_raw_MASK	GENMASK(27, 27)
#define DPU_INTP_REG__b_cmb_te_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_te_secu_int_raw_MASK	GENMASK(28, 28)
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_vsync_updt_secu_int_raw_MASK	GENMASK(29, 29)
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_int_raw_WORD	46
#define DPU_INTP_REG__b_cmb_vsync_done_free_secu_int_raw_MASK	GENMASK(30, 30)
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_int_raw_WORD	47
#define DPU_INTP_REG__b_cmb_cmdlist_ch_enter_pend_secu_int_raw_MASK	GENMASK(7, 0)
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_int_raw_WORD	47
#define DPU_INTP_REG__b_cmb_arb_dec_pslverr_secu_int_raw_MASK	GENMASK(8, 8)
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_int_raw_WORD	47
#define DPU_INTP_REG__b_cmb_cfg_eof_free_secu_int_raw_MASK	GENMASK(10, 9)
#define DPU_INTP_REG__b_cmb_eof_free_secu_int_raw_WORD	47
#define DPU_INTP_REG__b_cmb_eof_free_secu_int_raw_MASK	GENMASK(11, 11)
#define DPU_INTP_REG__b_cmb_sof_free_secu_int_raw_WORD	47
#define DPU_INTP_REG__b_cmb_sof_free_secu_int_raw_MASK	GENMASK(12, 12)
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_int_raw_WORD	48
#define DPU_INTP_REG__b_cmb_dma_dbg_secu_int_raw_MASK	GENMASK(15, 0)
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_int_raw_WORD	48
#define DPU_INTP_REG__b_cmb_outctl_dbg_secu_int_raw_MASK	GENMASK(16, 16)
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_int_raw_WORD	48
#define DPU_INTP_REG__b_cmb_ctl_dbg_secu_int_raw_MASK	GENMASK(17, 17)
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_int_raw_WORD	48
#define DPU_INTP_REG__b_cmb_cmdlist_dbg_secu_int_raw_MASK	GENMASK(18, 18)
#define DPU_INTP_REG__b_cmb_secu_arb_dec_paddr_WORD	49
#define DPU_INTP_REG__b_cmb_secu_arb_dec_paddr_MASK	GENMASK(20, 0)

/* EE_REG (ee.h): 51 x 32-bit registers */
#define EE_REG__NWORDS	51
#define EE_REG__m_benable_WORD	0
#define EE_REG__m_benable_MASK	GENMASK(0, 0)
#define EE_REG__m_plpf1_0_WORD	1
#define EE_REG__m_plpf1_0_MASK	GENMASK(7, 0)
#define EE_REG__m_plpf1_1_WORD	1
#define EE_REG__m_plpf1_1_MASK	GENMASK(15, 8)
#define EE_REG__m_plpf2_0_WORD	1
#define EE_REG__m_plpf2_0_MASK	GENMASK(23, 16)
#define EE_REG__m_plpf2_1_WORD	1
#define EE_REG__m_plpf2_1_MASK	GENMASK(31, 24)
#define EE_REG__m_nfreq_exp_level1_WORD	2
#define EE_REG__m_nfreq_exp_level1_MASK	GENMASK(5, 0)
#define EE_REG__m_nfreq_exp_level2_WORD	2
#define EE_REG__m_nfreq_exp_level2_MASK	GENMASK(13, 8)
#define EE_REG__m_nnode_freq_WORD	2
#define EE_REG__m_nnode_freq_MASK	GENMASK(20, 16)
#define EE_REG__m_nsingular_p1_WORD	3
#define EE_REG__m_nsingular_p1_MASK	GENMASK(12, 0)
#define EE_REG__m_nsingular_p2_WORD	3
#define EE_REG__m_nsingular_p2_MASK	GENMASK(28, 16)
#define EE_REG__m_pcurve_freq_tx_p_0_WORD	4
#define EE_REG__m_pcurve_freq_tx_p_0_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_p_1_WORD	4
#define EE_REG__m_pcurve_freq_tx_p_1_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_p_2_WORD	4
#define EE_REG__m_pcurve_freq_tx_p_2_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_p_3_WORD	4
#define EE_REG__m_pcurve_freq_tx_p_3_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_p_4_WORD	5
#define EE_REG__m_pcurve_freq_tx_p_4_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_p_5_WORD	5
#define EE_REG__m_pcurve_freq_tx_p_5_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_p_6_WORD	5
#define EE_REG__m_pcurve_freq_tx_p_6_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_p_7_WORD	5
#define EE_REG__m_pcurve_freq_tx_p_7_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_p_8_WORD	6
#define EE_REG__m_pcurve_freq_tx_p_8_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_p_9_WORD	6
#define EE_REG__m_pcurve_freq_tx_p_9_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_p_10_WORD	6
#define EE_REG__m_pcurve_freq_tx_p_10_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_p_11_WORD	6
#define EE_REG__m_pcurve_freq_tx_p_11_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_p_12_WORD	7
#define EE_REG__m_pcurve_freq_tx_p_12_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_p_13_WORD	7
#define EE_REG__m_pcurve_freq_tx_p_13_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_p_14_WORD	7
#define EE_REG__m_pcurve_freq_tx_p_14_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_p_15_WORD	7
#define EE_REG__m_pcurve_freq_tx_p_15_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_p_16_WORD	8
#define EE_REG__m_pcurve_freq_tx_p_16_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_n_0_WORD	9
#define EE_REG__m_pcurve_freq_tx_n_0_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_n_1_WORD	9
#define EE_REG__m_pcurve_freq_tx_n_1_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_n_2_WORD	9
#define EE_REG__m_pcurve_freq_tx_n_2_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_n_3_WORD	9
#define EE_REG__m_pcurve_freq_tx_n_3_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_n_4_WORD	10
#define EE_REG__m_pcurve_freq_tx_n_4_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_n_5_WORD	10
#define EE_REG__m_pcurve_freq_tx_n_5_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_n_6_WORD	10
#define EE_REG__m_pcurve_freq_tx_n_6_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_n_7_WORD	10
#define EE_REG__m_pcurve_freq_tx_n_7_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_n_8_WORD	11
#define EE_REG__m_pcurve_freq_tx_n_8_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_n_9_WORD	11
#define EE_REG__m_pcurve_freq_tx_n_9_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_n_10_WORD	11
#define EE_REG__m_pcurve_freq_tx_n_10_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_n_11_WORD	11
#define EE_REG__m_pcurve_freq_tx_n_11_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_n_12_WORD	12
#define EE_REG__m_pcurve_freq_tx_n_12_MASK	GENMASK(4, 0)
#define EE_REG__m_pcurve_freq_tx_n_13_WORD	12
#define EE_REG__m_pcurve_freq_tx_n_13_MASK	GENMASK(12, 8)
#define EE_REG__m_pcurve_freq_tx_n_14_WORD	12
#define EE_REG__m_pcurve_freq_tx_n_14_MASK	GENMASK(20, 16)
#define EE_REG__m_pcurve_freq_tx_n_15_WORD	12
#define EE_REG__m_pcurve_freq_tx_n_15_MASK	GENMASK(28, 24)
#define EE_REG__m_pcurve_freq_tx_n_16_WORD	13
#define EE_REG__m_pcurve_freq_tx_n_16_MASK	GENMASK(4, 0)
#define EE_REG__m_nfreq_offset_tx_WORD	14
#define EE_REG__m_nfreq_offset_tx_MASK	GENMASK(7, 0)
#define EE_REG__m_pcurve_luma_p_0_WORD	15
#define EE_REG__m_pcurve_luma_p_0_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_p_1_WORD	15
#define EE_REG__m_pcurve_luma_p_1_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_p_2_WORD	15
#define EE_REG__m_pcurve_luma_p_2_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_p_3_WORD	15
#define EE_REG__m_pcurve_luma_p_3_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_p_4_WORD	16
#define EE_REG__m_pcurve_luma_p_4_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_p_5_WORD	16
#define EE_REG__m_pcurve_luma_p_5_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_p_6_WORD	16
#define EE_REG__m_pcurve_luma_p_6_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_p_7_WORD	16
#define EE_REG__m_pcurve_luma_p_7_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_p_8_WORD	17
#define EE_REG__m_pcurve_luma_p_8_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_p_9_WORD	17
#define EE_REG__m_pcurve_luma_p_9_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_p_10_WORD	17
#define EE_REG__m_pcurve_luma_p_10_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_p_11_WORD	17
#define EE_REG__m_pcurve_luma_p_11_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_p_12_WORD	18
#define EE_REG__m_pcurve_luma_p_12_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_p_13_WORD	18
#define EE_REG__m_pcurve_luma_p_13_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_p_14_WORD	18
#define EE_REG__m_pcurve_luma_p_14_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_p_15_WORD	18
#define EE_REG__m_pcurve_luma_p_15_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_p_16_WORD	19
#define EE_REG__m_pcurve_luma_p_16_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_n_0_WORD	20
#define EE_REG__m_pcurve_luma_n_0_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_n_1_WORD	20
#define EE_REG__m_pcurve_luma_n_1_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_n_2_WORD	20
#define EE_REG__m_pcurve_luma_n_2_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_n_3_WORD	20
#define EE_REG__m_pcurve_luma_n_3_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_n_4_WORD	21
#define EE_REG__m_pcurve_luma_n_4_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_n_5_WORD	21
#define EE_REG__m_pcurve_luma_n_5_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_n_6_WORD	21
#define EE_REG__m_pcurve_luma_n_6_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_n_7_WORD	21
#define EE_REG__m_pcurve_luma_n_7_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_n_8_WORD	22
#define EE_REG__m_pcurve_luma_n_8_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_n_9_WORD	22
#define EE_REG__m_pcurve_luma_n_9_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_n_10_WORD	22
#define EE_REG__m_pcurve_luma_n_10_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_n_11_WORD	22
#define EE_REG__m_pcurve_luma_n_11_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_n_12_WORD	23
#define EE_REG__m_pcurve_luma_n_12_MASK	GENMASK(6, 0)
#define EE_REG__m_pcurve_luma_n_13_WORD	23
#define EE_REG__m_pcurve_luma_n_13_MASK	GENMASK(14, 8)
#define EE_REG__m_pcurve_luma_n_14_WORD	23
#define EE_REG__m_pcurve_luma_n_14_MASK	GENMASK(22, 16)
#define EE_REG__m_pcurve_luma_n_15_WORD	23
#define EE_REG__m_pcurve_luma_n_15_MASK	GENMASK(30, 24)
#define EE_REG__m_pcurve_luma_n_16_WORD	24
#define EE_REG__m_pcurve_luma_n_16_MASK	GENMASK(6, 0)
#define EE_REG__m_ntx_clip_p12_WORD	25
#define EE_REG__m_ntx_clip_p12_MASK	GENMASK(12, 0)
#define EE_REG__m_ntx_clip_p10_WORD	25
#define EE_REG__m_ntx_clip_p10_MASK	GENMASK(28, 16)
#define EE_REG__m_ntx_clip_n12_WORD	26
#define EE_REG__m_ntx_clip_n12_MASK	GENMASK(12, 0)
#define EE_REG__m_ntx_clip_n10_WORD	26
#define EE_REG__m_ntx_clip_n10_MASK	GENMASK(28, 16)
#define EE_REG__m_ntx_thrd_p12_WORD	27
#define EE_REG__m_ntx_thrd_p12_MASK	GENMASK(8, 0)
#define EE_REG__m_ntx_thrd_p10_WORD	27
#define EE_REG__m_ntx_thrd_p10_MASK	GENMASK(24, 16)
#define EE_REG__m_ntx_thrd_n12_WORD	28
#define EE_REG__m_ntx_thrd_n12_MASK	GENMASK(8, 0)
#define EE_REG__m_ntx_thrd_n10_WORD	28
#define EE_REG__m_ntx_thrd_n10_MASK	GENMASK(24, 16)
#define EE_REG__m_nclip_pos12_WORD	29
#define EE_REG__m_nclip_pos12_MASK	GENMASK(11, 0)
#define EE_REG__m_nclip_pos10_WORD	29
#define EE_REG__m_nclip_pos10_MASK	GENMASK(27, 16)
#define EE_REG__m_nclip_neg12_WORD	30
#define EE_REG__m_nclip_neg12_MASK	GENMASK(11, 0)
#define EE_REG__m_nclip_neg10_WORD	30
#define EE_REG__m_nclip_neg10_MASK	GENMASK(27, 16)
#define EE_REG__m_nhc_strength_WORD	31
#define EE_REG__m_nhc_strength_MASK	GENMASK(5, 0)
#define EE_REG__m_pcoff_w_0_WORD	31
#define EE_REG__m_pcoff_w_0_MASK	GENMASK(15, 8)
#define EE_REG__m_pcoff_w_1_WORD	31
#define EE_REG__m_pcoff_w_1_MASK	GENMASK(23, 16)
#define EE_REG__m_ncoff_w_curve_WORD	32
#define EE_REG__m_ncoff_w_curve_MASK	GENMASK(10, 0)
#define EE_REG__rgb2yuv_matrix00_WORD	33
#define EE_REG__rgb2yuv_matrix00_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix01_WORD	33
#define EE_REG__rgb2yuv_matrix01_MASK	GENMASK(31, 16)
#define EE_REG__rgb2yuv_matrix02_WORD	34
#define EE_REG__rgb2yuv_matrix02_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix03_WORD	34
#define EE_REG__rgb2yuv_matrix03_MASK	GENMASK(31, 16)
#define EE_REG__rgb2yuv_matrix10_WORD	35
#define EE_REG__rgb2yuv_matrix10_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix11_WORD	35
#define EE_REG__rgb2yuv_matrix11_MASK	GENMASK(31, 16)
#define EE_REG__rgb2yuv_matrix12_WORD	36
#define EE_REG__rgb2yuv_matrix12_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix13_WORD	36
#define EE_REG__rgb2yuv_matrix13_MASK	GENMASK(31, 16)
#define EE_REG__rgb2yuv_matrix20_WORD	37
#define EE_REG__rgb2yuv_matrix20_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix21_WORD	37
#define EE_REG__rgb2yuv_matrix21_MASK	GENMASK(31, 16)
#define EE_REG__rgb2yuv_matrix22_WORD	38
#define EE_REG__rgb2yuv_matrix22_MASK	GENMASK(15, 0)
#define EE_REG__rgb2yuv_matrix23_WORD	38
#define EE_REG__rgb2yuv_matrix23_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix00_WORD	39
#define EE_REG__yuv2rgb_matrix00_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix01_WORD	39
#define EE_REG__yuv2rgb_matrix01_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix02_WORD	40
#define EE_REG__yuv2rgb_matrix02_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix03_WORD	40
#define EE_REG__yuv2rgb_matrix03_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix10_WORD	41
#define EE_REG__yuv2rgb_matrix10_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix11_WORD	41
#define EE_REG__yuv2rgb_matrix11_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix12_WORD	42
#define EE_REG__yuv2rgb_matrix12_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix13_WORD	42
#define EE_REG__yuv2rgb_matrix13_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix20_WORD	43
#define EE_REG__yuv2rgb_matrix20_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix21_WORD	43
#define EE_REG__yuv2rgb_matrix21_MASK	GENMASK(31, 16)
#define EE_REG__yuv2rgb_matrix22_WORD	44
#define EE_REG__yuv2rgb_matrix22_MASK	GENMASK(15, 0)
#define EE_REG__yuv2rgb_matrix23_WORD	44
#define EE_REG__yuv2rgb_matrix23_MASK	GENMASK(31, 16)
#define EE_REG__cfg_se_WORD	45
#define EE_REG__cfg_se_MASK	GENMASK(0, 0)
#define EE_REG__force_update_en_WORD	46
#define EE_REG__force_update_en_MASK	GENMASK(0, 0)
#define EE_REG__vsync_update_en_WORD	46
#define EE_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define EE_REG__shadow_read_en_WORD	46
#define EE_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define EE_REG__force_update_pulse_WORD	47
#define EE_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define EE_REG__icg_override_WORD	48
#define EE_REG__icg_override_MASK	GENMASK(0, 0)
#define EE_REG__trigger_WORD	49
#define EE_REG__trigger_MASK	GENMASK(0, 0)
#define EE_REG__trigger2_WORD	50
#define EE_REG__trigger2_MASK	GENMASK(0, 0)

/* MMU_TBU_REG (mmu_tbu_x.h): 21 x 32-bit registers */
#define MMU_TBU_REG__NWORDS	21
#define MMU_TBU_REG__tbu_en_WORD	0
#define MMU_TBU_REG__tbu_en_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__tbu_fbc_mode_WORD	0
#define MMU_TBU_REG__tbu_fbc_mode_MASK	GENMASK(1, 1)
#define MMU_TBU_REG__tbu_plane_num_WORD	0
#define MMU_TBU_REG__tbu_plane_num_MASK	GENMASK(3, 2)
#define MMU_TBU_REG__tbu_burst_limit_en_WORD	0
#define MMU_TBU_REG__tbu_burst_limit_en_MASK	GENMASK(4, 4)
#define MMU_TBU_REG__tlb_fetch_active_en_WORD	0
#define MMU_TBU_REG__tlb_fetch_active_en_MASK	GENMASK(5, 5)
#define MMU_TBU_REG__tbu_qos_WORD	0
#define MMU_TBU_REG__tbu_qos_MASK	GENMASK(11, 8)
#define MMU_TBU_REG__tbu_base_addr0_low_WORD	1
#define MMU_TBU_REG__tbu_base_addr0_low_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_base_addr0_high_WORD	2
#define MMU_TBU_REG__tbu_base_addr0_high_MASK	GENMASK(11, 0)
#define MMU_TBU_REG__tbu_base_addr1_low_WORD	3
#define MMU_TBU_REG__tbu_base_addr1_low_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_base_addr1_high_WORD	4
#define MMU_TBU_REG__tbu_base_addr1_high_MASK	GENMASK(11, 0)
#define MMU_TBU_REG__tbu_base_addr2_low_WORD	5
#define MMU_TBU_REG__tbu_base_addr2_low_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_base_addr2_high_WORD	6
#define MMU_TBU_REG__tbu_base_addr2_high_MASK	GENMASK(11, 0)
#define MMU_TBU_REG__tbu_va0_WORD	7
#define MMU_TBU_REG__tbu_va0_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_va1_WORD	8
#define MMU_TBU_REG__tbu_va1_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_va2_WORD	9
#define MMU_TBU_REG__tbu_va2_MASK	GENMASK(31, 0)
#define MMU_TBU_REG__tbu_size0_WORD	10
#define MMU_TBU_REG__tbu_size0_MASK	GENMASK(15, 0)
#define MMU_TBU_REG__tbu_size1_WORD	11
#define MMU_TBU_REG__tbu_size1_MASK	GENMASK(15, 0)
#define MMU_TBU_REG__tbu_size2_WORD	12
#define MMU_TBU_REG__tbu_size2_MASK	GENMASK(15, 0)
#define MMU_TBU_REG__force_update_en_WORD	13
#define MMU_TBU_REG__force_update_en_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__vsync_update_en_WORD	13
#define MMU_TBU_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define MMU_TBU_REG__shadow_read_en_WORD	13
#define MMU_TBU_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define MMU_TBU_REG__force_update_pulse_WORD	14
#define MMU_TBU_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__force_update_en_se_WORD	15
#define MMU_TBU_REG__force_update_en_se_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__vsync_update_en_se_WORD	15
#define MMU_TBU_REG__vsync_update_en_se_MASK	GENMASK(1, 1)
#define MMU_TBU_REG__shadow_read_en_se_WORD	15
#define MMU_TBU_REG__shadow_read_en_se_MASK	GENMASK(2, 2)
#define MMU_TBU_REG__force_update_pulse_se_WORD	16
#define MMU_TBU_REG__force_update_pulse_se_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__icg_override_WORD	17
#define MMU_TBU_REG__icg_override_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__regbank_reset_WORD	18
#define MMU_TBU_REG__regbank_reset_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__regbank_reset2_WORD	19
#define MMU_TBU_REG__regbank_reset2_MASK	GENMASK(0, 0)
#define MMU_TBU_REG__tbu_nsaid_WORD	20
#define MMU_TBU_REG__tbu_nsaid_MASK	GENMASK(15, 0)

/* MMU_TOP_REG (mmu_top.h): 23 x 32-bit registers */
#define MMU_TOP_REG__NWORDS	23
#define MMU_TOP_REG__burst_split_mode_WORD	0
#define MMU_TOP_REG__burst_split_mode_MASK	GENMASK(0, 0)
#define MMU_TOP_REG__burst_split_en_WORD	0
#define MMU_TOP_REG__burst_split_en_MASK	GENMASK(1, 1)
#define MMU_TOP_REG__rdma_timelimit_WORD	0
#define MMU_TOP_REG__rdma_timelimit_MASK	GENMASK(31, 16)
#define MMU_TOP_REG__sram0_tlb_axi_port_sel_WORD	1
#define MMU_TOP_REG__sram0_tlb_axi_port_sel_MASK	GENMASK(1, 0)
#define MMU_TOP_REG__sram1_tlb_axi_port_sel_WORD	1
#define MMU_TOP_REG__sram1_tlb_axi_port_sel_MASK	GENMASK(3, 2)
#define MMU_TOP_REG__dmac0_rd_outs_num_WORD	2
#define MMU_TOP_REG__dmac0_rd_outs_num_MASK	GENMASK(7, 0)
#define MMU_TOP_REG__dmac0_arregion_WORD	2
#define MMU_TOP_REG__dmac0_arregion_MASK	GENMASK(11, 8)
#define MMU_TOP_REG__dmac0_arcache_WORD	2
#define MMU_TOP_REG__dmac0_arcache_MASK	GENMASK(15, 12)
#define MMU_TOP_REG__dmac0_burst_length_WORD	2
#define MMU_TOP_REG__dmac0_burst_length_MASK	GENMASK(18, 16)
#define MMU_TOP_REG__dmac1_rd_outs_num_WORD	3
#define MMU_TOP_REG__dmac1_rd_outs_num_MASK	GENMASK(7, 0)
#define MMU_TOP_REG__dmac1_arregion_WORD	3
#define MMU_TOP_REG__dmac1_arregion_MASK	GENMASK(11, 8)
#define MMU_TOP_REG__dmac1_arcache_WORD	3
#define MMU_TOP_REG__dmac1_arcache_MASK	GENMASK(15, 12)
#define MMU_TOP_REG__dmac1_burst_length_WORD	3
#define MMU_TOP_REG__dmac1_burst_length_MASK	GENMASK(18, 16)
#define MMU_TOP_REG__axi_s0_ar_debug_WORD	4
#define MMU_TOP_REG__axi_s0_ar_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_m0_ar_debug_WORD	5
#define MMU_TOP_REG__axi_m0_ar_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_s0_aw_debug_WORD	6
#define MMU_TOP_REG__axi_s0_aw_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_m0_aw_debug_WORD	7
#define MMU_TOP_REG__axi_m0_aw_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_s1_ar_debug_WORD	8
#define MMU_TOP_REG__axi_s1_ar_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_m1_ar_debug_WORD	9
#define MMU_TOP_REG__axi_m1_ar_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_s1_aw_debug_WORD	10
#define MMU_TOP_REG__axi_s1_aw_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_m1_aw_debug_WORD	11
#define MMU_TOP_REG__axi_m1_aw_debug_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__tlb_cmd_num_WORD	12
#define MMU_TOP_REG__tlb_cmd_num_MASK	GENMASK(19, 0)
#define MMU_TOP_REG__tlb_miss_num_WORD	12
#define MMU_TOP_REG__tlb_miss_num_MASK	GENMASK(31, 20)
#define MMU_TOP_REG__tlb_wait_cycle_WORD	13
#define MMU_TOP_REG__tlb_wait_cycle_MASK	GENMASK(15, 0)
#define MMU_TOP_REG__tlb_wait_cycle_sel_WORD	14
#define MMU_TOP_REG__tlb_wait_cycle_sel_MASK	GENMASK(6, 0)
#define MMU_TOP_REG__tlb_cmd_num_sel_WORD	14
#define MMU_TOP_REG__tlb_cmd_num_sel_MASK	GENMASK(14, 8)
#define MMU_TOP_REG__tlb_miss_num_sel_WORD	14
#define MMU_TOP_REG__tlb_miss_num_sel_MASK	GENMASK(22, 16)
#define MMU_TOP_REG__tlb_miss_num_clr_WORD	14
#define MMU_TOP_REG__tlb_miss_num_clr_MASK	GENMASK(23, 23)
#define MMU_TOP_REG__tlb_wait_cycle_total_WORD	15
#define MMU_TOP_REG__tlb_wait_cycle_total_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__tlb_cmd_num_total_WORD	16
#define MMU_TOP_REG__tlb_cmd_num_total_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__tlb_miss_num_total_WORD	17
#define MMU_TOP_REG__tlb_miss_num_total_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__icg_override_WORD	18
#define MMU_TOP_REG__icg_override_MASK	GENMASK(0, 0)
#define MMU_TOP_REG__regbank_reset_WORD	19
#define MMU_TOP_REG__regbank_reset_MASK	GENMASK(0, 0)
#define MMU_TOP_REG__regbank_reset2_WORD	20
#define MMU_TOP_REG__regbank_reset2_MASK	GENMASK(0, 0)
#define MMU_TOP_REG__axi_safty_addr_low_WORD	21
#define MMU_TOP_REG__axi_safty_addr_low_MASK	GENMASK(31, 0)
#define MMU_TOP_REG__axi_safty_addr_high_WORD	22
#define MMU_TOP_REG__axi_safty_addr_high_MASK	GENMASK(11, 0)

/* POSTPIPE_REG (postpipe.h): 50 x 32-bit registers */
#define POSTPIPE_REG__NWORDS	50
#define POSTPIPE_REG__dither_en_WORD	0
#define POSTPIPE_REG__dither_en_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__postpq_en_WORD	0
#define POSTPIPE_REG__postpq_en_MASK	GENMASK(1, 1)
#define POSTPIPE_REG__dither_mem_lp_en_WORD	0
#define POSTPIPE_REG__dither_mem_lp_en_MASK	GENMASK(2, 2)
#define POSTPIPE_REG__rgb2yuv_en_WORD	0
#define POSTPIPE_REG__rgb2yuv_en_MASK	GENMASK(3, 3)
#define POSTPIPE_REG__yuv2rgb_en_WORD	0
#define POSTPIPE_REG__yuv2rgb_en_MASK	GENMASK(4, 4)
#define POSTPIPE_REG__split_en_WORD	0
#define POSTPIPE_REG__split_en_MASK	GENMASK(5, 5)
#define POSTPIPE_REG__rgb2yuv_matrix00_WORD	1
#define POSTPIPE_REG__rgb2yuv_matrix00_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix01_WORD	1
#define POSTPIPE_REG__rgb2yuv_matrix01_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__rgb2yuv_matrix02_WORD	2
#define POSTPIPE_REG__rgb2yuv_matrix02_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix03_WORD	2
#define POSTPIPE_REG__rgb2yuv_matrix03_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__rgb2yuv_matrix10_WORD	3
#define POSTPIPE_REG__rgb2yuv_matrix10_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix11_WORD	3
#define POSTPIPE_REG__rgb2yuv_matrix11_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__rgb2yuv_matrix12_WORD	4
#define POSTPIPE_REG__rgb2yuv_matrix12_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix13_WORD	4
#define POSTPIPE_REG__rgb2yuv_matrix13_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__rgb2yuv_matrix20_WORD	5
#define POSTPIPE_REG__rgb2yuv_matrix20_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix21_WORD	5
#define POSTPIPE_REG__rgb2yuv_matrix21_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__rgb2yuv_matrix22_WORD	6
#define POSTPIPE_REG__rgb2yuv_matrix22_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__rgb2yuv_matrix23_WORD	6
#define POSTPIPE_REG__rgb2yuv_matrix23_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix00_WORD	7
#define POSTPIPE_REG__yuv2rgb_matrix00_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix01_WORD	7
#define POSTPIPE_REG__yuv2rgb_matrix01_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix02_WORD	8
#define POSTPIPE_REG__yuv2rgb_matrix02_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix03_WORD	8
#define POSTPIPE_REG__yuv2rgb_matrix03_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix10_WORD	9
#define POSTPIPE_REG__yuv2rgb_matrix10_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix11_WORD	9
#define POSTPIPE_REG__yuv2rgb_matrix11_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix12_WORD	10
#define POSTPIPE_REG__yuv2rgb_matrix12_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix13_WORD	10
#define POSTPIPE_REG__yuv2rgb_matrix13_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix20_WORD	11
#define POSTPIPE_REG__yuv2rgb_matrix20_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix21_WORD	11
#define POSTPIPE_REG__yuv2rgb_matrix21_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__yuv2rgb_matrix22_WORD	12
#define POSTPIPE_REG__yuv2rgb_matrix22_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__yuv2rgb_matrix23_WORD	12
#define POSTPIPE_REG__yuv2rgb_matrix23_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__m_inwidth_WORD	13
#define POSTPIPE_REG__m_inwidth_MASK	GENMASK(15, 0)
#define POSTPIPE_REG__m_inheight_WORD	13
#define POSTPIPE_REG__m_inheight_MASK	GENMASK(31, 16)
#define POSTPIPE_REG__split_overlap_WORD	14
#define POSTPIPE_REG__split_overlap_MASK	GENMASK(9, 0)
#define POSTPIPE_REG__dither_mode_WORD	19
#define POSTPIPE_REG__dither_mode_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__dither_auto_temp_WORD	19
#define POSTPIPE_REG__dither_auto_temp_MASK	GENMASK(1, 1)
#define POSTPIPE_REG__dither_rotate_mode_WORD	19
#define POSTPIPE_REG__dither_rotate_mode_MASK	GENMASK(3, 2)
#define POSTPIPE_REG__dither_out_dpth0_WORD	19
#define POSTPIPE_REG__dither_out_dpth0_MASK	GENMASK(7, 4)
#define POSTPIPE_REG__dither_out_dpth1_WORD	19
#define POSTPIPE_REG__dither_out_dpth1_MASK	GENMASK(11, 8)
#define POSTPIPE_REG__dither_out_dpth2_WORD	19
#define POSTPIPE_REG__dither_out_dpth2_MASK	GENMASK(15, 12)
#define POSTPIPE_REG__dither_temp_value_WORD	19
#define POSTPIPE_REG__dither_temp_value_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_pattern_bit_WORD	19
#define POSTPIPE_REG__dither_pattern_bit_MASK	GENMASK(26, 24)
#define POSTPIPE_REG__dither_bayer_map00_WORD	20
#define POSTPIPE_REG__dither_bayer_map00_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map01_WORD	20
#define POSTPIPE_REG__dither_bayer_map01_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map02_WORD	20
#define POSTPIPE_REG__dither_bayer_map02_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map03_WORD	20
#define POSTPIPE_REG__dither_bayer_map03_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map04_WORD	21
#define POSTPIPE_REG__dither_bayer_map04_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map05_WORD	21
#define POSTPIPE_REG__dither_bayer_map05_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map06_WORD	21
#define POSTPIPE_REG__dither_bayer_map06_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map07_WORD	21
#define POSTPIPE_REG__dither_bayer_map07_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map10_WORD	22
#define POSTPIPE_REG__dither_bayer_map10_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map11_WORD	22
#define POSTPIPE_REG__dither_bayer_map11_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map12_WORD	22
#define POSTPIPE_REG__dither_bayer_map12_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map13_WORD	22
#define POSTPIPE_REG__dither_bayer_map13_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map14_WORD	23
#define POSTPIPE_REG__dither_bayer_map14_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map15_WORD	23
#define POSTPIPE_REG__dither_bayer_map15_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map16_WORD	23
#define POSTPIPE_REG__dither_bayer_map16_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map17_WORD	23
#define POSTPIPE_REG__dither_bayer_map17_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map20_WORD	24
#define POSTPIPE_REG__dither_bayer_map20_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map21_WORD	24
#define POSTPIPE_REG__dither_bayer_map21_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map22_WORD	24
#define POSTPIPE_REG__dither_bayer_map22_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map23_WORD	24
#define POSTPIPE_REG__dither_bayer_map23_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map24_WORD	25
#define POSTPIPE_REG__dither_bayer_map24_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map25_WORD	25
#define POSTPIPE_REG__dither_bayer_map25_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map26_WORD	25
#define POSTPIPE_REG__dither_bayer_map26_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map27_WORD	25
#define POSTPIPE_REG__dither_bayer_map27_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map30_WORD	26
#define POSTPIPE_REG__dither_bayer_map30_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map31_WORD	26
#define POSTPIPE_REG__dither_bayer_map31_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map32_WORD	26
#define POSTPIPE_REG__dither_bayer_map32_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map33_WORD	26
#define POSTPIPE_REG__dither_bayer_map33_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map34_WORD	27
#define POSTPIPE_REG__dither_bayer_map34_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map35_WORD	27
#define POSTPIPE_REG__dither_bayer_map35_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map36_WORD	27
#define POSTPIPE_REG__dither_bayer_map36_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map37_WORD	27
#define POSTPIPE_REG__dither_bayer_map37_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map40_WORD	28
#define POSTPIPE_REG__dither_bayer_map40_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map41_WORD	28
#define POSTPIPE_REG__dither_bayer_map41_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map42_WORD	28
#define POSTPIPE_REG__dither_bayer_map42_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map43_WORD	28
#define POSTPIPE_REG__dither_bayer_map43_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map44_WORD	29
#define POSTPIPE_REG__dither_bayer_map44_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map45_WORD	29
#define POSTPIPE_REG__dither_bayer_map45_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map46_WORD	29
#define POSTPIPE_REG__dither_bayer_map46_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map47_WORD	29
#define POSTPIPE_REG__dither_bayer_map47_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map50_WORD	30
#define POSTPIPE_REG__dither_bayer_map50_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map51_WORD	30
#define POSTPIPE_REG__dither_bayer_map51_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map52_WORD	30
#define POSTPIPE_REG__dither_bayer_map52_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map53_WORD	30
#define POSTPIPE_REG__dither_bayer_map53_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map54_WORD	31
#define POSTPIPE_REG__dither_bayer_map54_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map55_WORD	31
#define POSTPIPE_REG__dither_bayer_map55_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map56_WORD	31
#define POSTPIPE_REG__dither_bayer_map56_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map57_WORD	31
#define POSTPIPE_REG__dither_bayer_map57_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map60_WORD	32
#define POSTPIPE_REG__dither_bayer_map60_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map61_WORD	32
#define POSTPIPE_REG__dither_bayer_map61_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map62_WORD	32
#define POSTPIPE_REG__dither_bayer_map62_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map63_WORD	32
#define POSTPIPE_REG__dither_bayer_map63_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map64_WORD	33
#define POSTPIPE_REG__dither_bayer_map64_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map65_WORD	33
#define POSTPIPE_REG__dither_bayer_map65_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map66_WORD	33
#define POSTPIPE_REG__dither_bayer_map66_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map67_WORD	33
#define POSTPIPE_REG__dither_bayer_map67_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map70_WORD	34
#define POSTPIPE_REG__dither_bayer_map70_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map71_WORD	34
#define POSTPIPE_REG__dither_bayer_map71_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map72_WORD	34
#define POSTPIPE_REG__dither_bayer_map72_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map73_WORD	34
#define POSTPIPE_REG__dither_bayer_map73_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__dither_bayer_map74_WORD	35
#define POSTPIPE_REG__dither_bayer_map74_MASK	GENMASK(7, 0)
#define POSTPIPE_REG__dither_bayer_map75_WORD	35
#define POSTPIPE_REG__dither_bayer_map75_MASK	GENMASK(15, 8)
#define POSTPIPE_REG__dither_bayer_map76_WORD	35
#define POSTPIPE_REG__dither_bayer_map76_MASK	GENMASK(23, 16)
#define POSTPIPE_REG__dither_bayer_map77_WORD	35
#define POSTPIPE_REG__dither_bayer_map77_MASK	GENMASK(31, 24)
#define POSTPIPE_REG__preproc_loc_WORD	36
#define POSTPIPE_REG__preproc_loc_MASK	GENMASK(31, 0)
#define POSTPIPE_REG__dither_proc_loc_WORD	37
#define POSTPIPE_REG__dither_proc_loc_MASK	GENMASK(31, 0)
#define POSTPIPE_REG__split_location_WORD	38
#define POSTPIPE_REG__split_location_MASK	GENMASK(31, 0)
#define POSTPIPE_REG__p0_out_location_WORD	39
#define POSTPIPE_REG__p0_out_location_MASK	GENMASK(31, 0)
#define POSTPIPE_REG__p1_out_location_WORD	40
#define POSTPIPE_REG__p1_out_location_MASK	GENMASK(31, 0)
#define POSTPIPE_REG__pospq_valid_WORD	41
#define POSTPIPE_REG__pospq_valid_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__pospq_ready_WORD	41
#define POSTPIPE_REG__pospq_ready_MASK	GENMASK(1, 1)
#define POSTPIPE_REG__acad_valid_WORD	41
#define POSTPIPE_REG__acad_valid_MASK	GENMASK(2, 2)
#define POSTPIPE_REG__acad_ready_WORD	41
#define POSTPIPE_REG__acad_ready_MASK	GENMASK(3, 3)
#define POSTPIPE_REG__hsv_valid_WORD	41
#define POSTPIPE_REG__hsv_valid_MASK	GENMASK(4, 4)
#define POSTPIPE_REG__hsv_ready_WORD	41
#define POSTPIPE_REG__hsv_ready_MASK	GENMASK(5, 5)
#define POSTPIPE_REG__endmatrix_valid_WORD	41
#define POSTPIPE_REG__endmatrix_valid_MASK	GENMASK(6, 6)
#define POSTPIPE_REG__endmatrix_ready_WORD	41
#define POSTPIPE_REG__endmatrix_ready_MASK	GENMASK(7, 7)
#define POSTPIPE_REG__to_scl_valid_WORD	41
#define POSTPIPE_REG__to_scl_valid_MASK	GENMASK(8, 8)
#define POSTPIPE_REG__scl_ready_WORD	41
#define POSTPIPE_REG__scl_ready_MASK	GENMASK(9, 9)
#define POSTPIPE_REG__lut3d_valid_WORD	41
#define POSTPIPE_REG__lut3d_valid_MASK	GENMASK(10, 10)
#define POSTPIPE_REG__lut3d_ready_WORD	41
#define POSTPIPE_REG__lut3d_ready_MASK	GENMASK(11, 11)
#define POSTPIPE_REG__ee_valid_WORD	41
#define POSTPIPE_REG__ee_valid_MASK	GENMASK(12, 12)
#define POSTPIPE_REG__ee_ready_WORD	41
#define POSTPIPE_REG__ee_ready_MASK	GENMASK(13, 13)
#define POSTPIPE_REG__usrgamma_valid_WORD	41
#define POSTPIPE_REG__usrgamma_valid_MASK	GENMASK(14, 14)
#define POSTPIPE_REG__usrgamma_ready_WORD	41
#define POSTPIPE_REG__usrgamma_ready_MASK	GENMASK(15, 15)
#define POSTPIPE_REG__dither_valid_WORD	41
#define POSTPIPE_REG__dither_valid_MASK	GENMASK(16, 16)
#define POSTPIPE_REG__dither_ready_WORD	41
#define POSTPIPE_REG__dither_ready_MASK	GENMASK(17, 17)
#define POSTPIPE_REG__rc_valid_WORD	41
#define POSTPIPE_REG__rc_valid_MASK	GENMASK(18, 18)
#define POSTPIPE_REG__rc_ready_WORD	41
#define POSTPIPE_REG__rc_ready_MASK	GENMASK(19, 19)
#define POSTPIPE_REG__split_valid_WORD	41
#define POSTPIPE_REG__split_valid_MASK	GENMASK(20, 20)
#define POSTPIPE_REG__split_ready_WORD	41
#define POSTPIPE_REG__split_ready_MASK	GENMASK(21, 21)
#define POSTPIPE_REG__dsc_p0_valid_WORD	41
#define POSTPIPE_REG__dsc_p0_valid_MASK	GENMASK(22, 22)
#define POSTPIPE_REG__dsc_p0_ready_WORD	41
#define POSTPIPE_REG__dsc_p0_ready_MASK	GENMASK(23, 23)
#define POSTPIPE_REG__dsc_p1_valid_WORD	41
#define POSTPIPE_REG__dsc_p1_valid_MASK	GENMASK(24, 24)
#define POSTPIPE_REG__dsc_p1_ready_WORD	41
#define POSTPIPE_REG__dsc_p1_ready_MASK	GENMASK(25, 25)
#define POSTPIPE_REG__postpipe_p0_valid_WORD	41
#define POSTPIPE_REG__postpipe_p0_valid_MASK	GENMASK(26, 26)
#define POSTPIPE_REG__postpipe_p0_ready_WORD	41
#define POSTPIPE_REG__postpipe_p0_ready_MASK	GENMASK(27, 27)
#define POSTPIPE_REG__postpipe_p1_valid_WORD	41
#define POSTPIPE_REG__postpipe_p1_valid_MASK	GENMASK(28, 28)
#define POSTPIPE_REG__postpipe_p1_ready_WORD	41
#define POSTPIPE_REG__postpipe_p1_ready_MASK	GENMASK(29, 29)
#define POSTPIPE_REG__force_update_en_WORD	45
#define POSTPIPE_REG__force_update_en_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__vsync_update_en_WORD	45
#define POSTPIPE_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define POSTPIPE_REG__shadow_read_en_WORD	45
#define POSTPIPE_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define POSTPIPE_REG__force_update_pulse_WORD	46
#define POSTPIPE_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__icg_override_WORD	47
#define POSTPIPE_REG__icg_override_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__trigger_WORD	48
#define POSTPIPE_REG__trigger_MASK	GENMASK(0, 0)
#define POSTPIPE_REG__trigger2_WORD	49
#define POSTPIPE_REG__trigger2_MASK	GENMASK(0, 0)

/* RDMA_PATH_X_REG (rdma_path.h): 60 x 32-bit registers */
#define RDMA_PATH_X_REG__NWORDS	60
#define RDMA_PATH_X_REG__layer_mode_WORD	0
#define RDMA_PATH_X_REG__layer_mode_MASK	GENMASK(1, 0)
#define RDMA_PATH_X_REG__outstanding_num_WORD	0
#define RDMA_PATH_X_REG__outstanding_num_MASK	GENMASK(6, 2)
#define RDMA_PATH_X_REG__req_conti_num_WORD	0
#define RDMA_PATH_X_REG__req_conti_num_MASK	GENMASK(12, 8)
#define RDMA_PATH_X_REG__layer_cmpsr_id_WORD	0
#define RDMA_PATH_X_REG__layer_cmpsr_id_MASK	GENMASK(15, 13)
#define RDMA_PATH_X_REG__axi_port_sel_WORD	0
#define RDMA_PATH_X_REG__axi_port_sel_MASK	GENMASK(17, 16)
#define RDMA_PATH_X_REG__rdma_burst_len_WORD	0
#define RDMA_PATH_X_REG__rdma_burst_len_MASK	GENMASK(31, 24)
#define RDMA_PATH_X_REG__is_two_layers_WORD	1
#define RDMA_PATH_X_REG__is_two_layers_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__is_offline_WORD	1
#define RDMA_PATH_X_REG__is_offline_MASK	GENMASK(1, 1)
#define RDMA_PATH_X_REG__hdr_osd_num_WORD	1
#define RDMA_PATH_X_REG__hdr_osd_num_MASK	GENMASK(5, 2)
#define RDMA_PATH_X_REG__cmpsr_y_offset0_WORD	2
#define RDMA_PATH_X_REG__cmpsr_y_offset0_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__base_addr0_low_ly0_WORD	9
#define RDMA_PATH_X_REG__base_addr0_low_ly0_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__base_addr0_high_ly0_WORD	10
#define RDMA_PATH_X_REG__base_addr0_high_ly0_MASK	GENMASK(1, 0)
#define RDMA_PATH_X_REG__base_addr1_low_ly0_WORD	11
#define RDMA_PATH_X_REG__base_addr1_low_ly0_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__base_addr1_high_ly0_WORD	12
#define RDMA_PATH_X_REG__base_addr1_high_ly0_MASK	GENMASK(1, 0)
#define RDMA_PATH_X_REG__base_addr2_low_ly0_WORD	13
#define RDMA_PATH_X_REG__base_addr2_low_ly0_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__base_addr2_high_ly0_WORD	14
#define RDMA_PATH_X_REG__base_addr2_high_ly0_MASK	GENMASK(1, 0)
#define RDMA_PATH_X_REG__rdma_stride0_layer0_WORD	15
#define RDMA_PATH_X_REG__rdma_stride0_layer0_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__rdma_stride1_layer0_WORD	15
#define RDMA_PATH_X_REG__rdma_stride1_layer0_MASK	GENMASK(31, 16)
#define RDMA_PATH_X_REG__img_width_ly0_WORD	16
#define RDMA_PATH_X_REG__img_width_ly0_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__img_height_ly0_WORD	16
#define RDMA_PATH_X_REG__img_height_ly0_MASK	GENMASK(31, 16)
#define RDMA_PATH_X_REG__bbox_start_x_ly0_WORD	17
#define RDMA_PATH_X_REG__bbox_start_x_ly0_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__bbox_start_y_ly0_WORD	17
#define RDMA_PATH_X_REG__bbox_start_y_ly0_MASK	GENMASK(31, 16)
#define RDMA_PATH_X_REG__bbox_end_x_ly0_WORD	18
#define RDMA_PATH_X_REG__bbox_end_x_ly0_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__bbox_end_y_ly0_WORD	18
#define RDMA_PATH_X_REG__bbox_end_y_ly0_MASK	GENMASK(31, 16)
#define RDMA_PATH_X_REG__pixel_format_WORD	29
#define RDMA_PATH_X_REG__pixel_format_MASK	GENMASK(5, 0)
#define RDMA_PATH_X_REG__uv_swap_WORD	29
#define RDMA_PATH_X_REG__uv_swap_MASK	GENMASK(6, 6)
#define RDMA_PATH_X_REG__rot_mode_ly0_WORD	29
#define RDMA_PATH_X_REG__rot_mode_ly0_MASK	GENMASK(10, 8)
#define RDMA_PATH_X_REG__rot_mode_ly1_WORD	29
#define RDMA_PATH_X_REG__rot_mode_ly1_MASK	GENMASK(13, 11)
#define RDMA_PATH_X_REG__fbc_split_mode_WORD	30
#define RDMA_PATH_X_REG__fbc_split_mode_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__fbc_yuv_transform_WORD	30
#define RDMA_PATH_X_REG__fbc_yuv_transform_MASK	GENMASK(1, 1)
#define RDMA_PATH_X_REG__fbc_sb_layout_WORD	30
#define RDMA_PATH_X_REG__fbc_sb_layout_MASK	GENMASK(2, 2)
#define RDMA_PATH_X_REG__fbc_tile_type_WORD	30
#define RDMA_PATH_X_REG__fbc_tile_type_MASK	GENMASK(3, 3)
#define RDMA_PATH_X_REG__fbc_mem_size_WORD	31
#define RDMA_PATH_X_REG__fbc_mem_size_MASK	GENMASK(15, 0)
#define RDMA_PATH_X_REG__fbc_mem_base_addr_WORD	31
#define RDMA_PATH_X_REG__fbc_mem_base_addr_MASK	GENMASK(27, 16)
#define RDMA_PATH_X_REG__fbc_mem_map_WORD	31
#define RDMA_PATH_X_REG__fbc_mem_map_MASK	GENMASK(28, 28)
#define RDMA_PATH_X_REG__dec_line_num_sw_WORD	31
#define RDMA_PATH_X_REG__dec_line_num_sw_MASK	GENMASK(29, 29)
#define RDMA_PATH_X_REG__sw_dec_line_num_WORD	31
#define RDMA_PATH_X_REG__sw_dec_line_num_MASK	GENMASK(31, 30)
#define RDMA_PATH_X_REG__nsaid_WORD	32
#define RDMA_PATH_X_REG__nsaid_MASK	GENMASK(3, 0)
#define RDMA_PATH_X_REG__csc_matrix00_WORD	33
#define RDMA_PATH_X_REG__csc_matrix00_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix01_WORD	33
#define RDMA_PATH_X_REG__csc_matrix01_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__csc_matrix02_WORD	34
#define RDMA_PATH_X_REG__csc_matrix02_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix03_WORD	34
#define RDMA_PATH_X_REG__csc_matrix03_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__csc_matrix10_WORD	35
#define RDMA_PATH_X_REG__csc_matrix10_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix11_WORD	35
#define RDMA_PATH_X_REG__csc_matrix11_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__csc_matrix12_WORD	36
#define RDMA_PATH_X_REG__csc_matrix12_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix13_WORD	36
#define RDMA_PATH_X_REG__csc_matrix13_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__csc_matrix20_WORD	37
#define RDMA_PATH_X_REG__csc_matrix20_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix21_WORD	37
#define RDMA_PATH_X_REG__csc_matrix21_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__csc_matrix22_WORD	38
#define RDMA_PATH_X_REG__csc_matrix22_MASK	GENMASK(13, 0)
#define RDMA_PATH_X_REG__csc_matrix23_WORD	38
#define RDMA_PATH_X_REG__csc_matrix23_MASK	GENMASK(29, 16)
#define RDMA_PATH_X_REG__alpha0_ly0_WORD	39
#define RDMA_PATH_X_REG__alpha0_ly0_MASK	GENMASK(11, 0)
#define RDMA_PATH_X_REG__alpha1_ly0_WORD	39
#define RDMA_PATH_X_REG__alpha1_ly0_MASK	GENMASK(27, 16)
#define RDMA_PATH_X_REG__alpha2_ly0_WORD	40
#define RDMA_PATH_X_REG__alpha2_ly0_MASK	GENMASK(11, 0)
#define RDMA_PATH_X_REG__alpha3_ly0_WORD	40
#define RDMA_PATH_X_REG__alpha3_ly0_MASK	GENMASK(27, 16)
#define RDMA_PATH_X_REG__rdma_urgent_up_WORD	41
#define RDMA_PATH_X_REG__rdma_urgent_up_MASK	GENMASK(7, 0)
#define RDMA_PATH_X_REG__rdma_urgent_down_WORD	41
#define RDMA_PATH_X_REG__rdma_urgent_down_MASK	GENMASK(15, 8)
#define RDMA_PATH_X_REG__sw_qos_rsv0_WORD	42
#define RDMA_PATH_X_REG__sw_qos_rsv0_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__sw_urgent_en_WORD	42
#define RDMA_PATH_X_REG__sw_urgent_en_MASK	GENMASK(1, 1)
#define RDMA_PATH_X_REG__sw_urgent_WORD	42
#define RDMA_PATH_X_REG__sw_urgent_MASK	GENMASK(2, 2)
#define RDMA_PATH_X_REG__offl_prefetch_lines_WORD	42
#define RDMA_PATH_X_REG__offl_prefetch_lines_MASK	GENMASK(15, 8)
#define RDMA_PATH_X_REG__rdma_dbg_irq_raw_WORD	43
#define RDMA_PATH_X_REG__rdma_dbg_irq_raw_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_irq_mask_WORD	44
#define RDMA_PATH_X_REG__rdma_dbg_irq_mask_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_irq_status_WORD	45
#define RDMA_PATH_X_REG__rdma_dbg_irq_status_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__prefetch_osd_num_WORD	46
#define RDMA_PATH_X_REG__prefetch_osd_num_MASK	GENMASK(4, 0)
#define RDMA_PATH_X_REG__prefetch_hdr_osd_num_WORD	46
#define RDMA_PATH_X_REG__prefetch_hdr_osd_num_MASK	GENMASK(11, 8)
#define RDMA_PATH_X_REG__osd_sub_step_WORD	46
#define RDMA_PATH_X_REG__osd_sub_step_MASK	GENMASK(13, 12)
#define RDMA_PATH_X_REG__osd_num_min_WORD	46
#define RDMA_PATH_X_REG__osd_num_min_MASK	GENMASK(20, 16)
#define RDMA_PATH_X_REG__hdr_osd_num_min_WORD	46
#define RDMA_PATH_X_REG__hdr_osd_num_min_MASK	GENMASK(27, 24)
#define RDMA_PATH_X_REG__pp_buf_switch_num_WORD	46
#define RDMA_PATH_X_REG__pp_buf_switch_num_MASK	GENMASK(30, 28)
#define RDMA_PATH_X_REG__rdma_dbg_info0_WORD	47
#define RDMA_PATH_X_REG__rdma_dbg_info0_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_info1_WORD	48
#define RDMA_PATH_X_REG__rdma_dbg_info1_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_info2_WORD	49
#define RDMA_PATH_X_REG__rdma_dbg_info2_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_info3_WORD	50
#define RDMA_PATH_X_REG__rdma_dbg_info3_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__rdma_dbg_info4_WORD	51
#define RDMA_PATH_X_REG__rdma_dbg_info4_MASK	GENMASK(31, 0)
#define RDMA_PATH_X_REG__secu_en_WORD	52
#define RDMA_PATH_X_REG__secu_en_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__force_update_en_WORD	53
#define RDMA_PATH_X_REG__force_update_en_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__vsync_update_en_WORD	53
#define RDMA_PATH_X_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define RDMA_PATH_X_REG__shadow_read_en_WORD	53
#define RDMA_PATH_X_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define RDMA_PATH_X_REG__force_update_pulse_WORD	54
#define RDMA_PATH_X_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__force_update_en_se_WORD	55
#define RDMA_PATH_X_REG__force_update_en_se_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__vsync_update_en_se_WORD	55
#define RDMA_PATH_X_REG__vsync_update_en_se_MASK	GENMASK(1, 1)
#define RDMA_PATH_X_REG__shadow_read_en_se_WORD	55
#define RDMA_PATH_X_REG__shadow_read_en_se_MASK	GENMASK(2, 2)
#define RDMA_PATH_X_REG__force_update_pulse_se_WORD	56
#define RDMA_PATH_X_REG__force_update_pulse_se_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__icg_override_WORD	57
#define RDMA_PATH_X_REG__icg_override_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__regbank_reset_WORD	58
#define RDMA_PATH_X_REG__regbank_reset_MASK	GENMASK(0, 0)
#define RDMA_PATH_X_REG__regbank_reset2_WORD	59
#define RDMA_PATH_X_REG__regbank_reset2_MASK	GENMASK(0, 0)

/* SCALER_X_REG (scale_x.h): 129 x 32-bit registers */
#define SCALER_X_REG__NWORDS	129
#define SCALER_X_REG__b_m_nscl_hor_enable_WORD	0
#define SCALER_X_REG__b_m_nscl_hor_enable_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_m_nscl_ver_enable_WORD	0
#define SCALER_X_REG__b_m_nscl_ver_enable_MASK	GENMASK(1, 1)
#define SCALER_X_REG__b_m_nyuv_narrow_WORD	0
#define SCALER_X_REG__b_m_nyuv_narrow_MASK	GENMASK(2, 2)
#define SCALER_X_REG__b_m_nscl_input_width_WORD	1
#define SCALER_X_REG__b_m_nscl_input_width_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_input_height_WORD	1
#define SCALER_X_REG__b_m_nscl_input_height_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_output_width_WORD	2
#define SCALER_X_REG__b_m_nscl_output_width_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_output_height_WORD	2
#define SCALER_X_REG__b_m_nscl_output_height_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_init_phase_l32b_WORD	3
#define SCALER_X_REG__b_m_nscl_hor_init_phase_l32b_MASK	GENMASK(31, 0)
#define SCALER_X_REG__b_m_nscl_hor_init_phase_h1b_WORD	4
#define SCALER_X_REG__b_m_nscl_hor_init_phase_h1b_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_m_nscl_ver_init_phase_l32b_WORD	5
#define SCALER_X_REG__b_m_nscl_ver_init_phase_l32b_MASK	GENMASK(31, 0)
#define SCALER_X_REG__b_m_nscl_ver_init_phase_h1b_WORD	6
#define SCALER_X_REG__b_m_nscl_ver_init_phase_h1b_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_m_nscl_hor_delta_phase_WORD	7
#define SCALER_X_REG__b_m_nscl_hor_delta_phase_MASK	GENMASK(19, 0)
#define SCALER_X_REG__b_m_nscl_ver_delta_phase_WORD	8
#define SCALER_X_REG__b_m_nscl_ver_delta_phase_MASK	GENMASK(19, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef0_WORD	9
#define SCALER_X_REG__b_m_nscl_hor_coef0_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef1_WORD	9
#define SCALER_X_REG__b_m_nscl_hor_coef1_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef2_WORD	10
#define SCALER_X_REG__b_m_nscl_hor_coef2_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef3_WORD	10
#define SCALER_X_REG__b_m_nscl_hor_coef3_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef4_WORD	11
#define SCALER_X_REG__b_m_nscl_hor_coef4_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef5_WORD	11
#define SCALER_X_REG__b_m_nscl_hor_coef5_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef6_WORD	12
#define SCALER_X_REG__b_m_nscl_hor_coef6_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef7_WORD	12
#define SCALER_X_REG__b_m_nscl_hor_coef7_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef8_WORD	13
#define SCALER_X_REG__b_m_nscl_hor_coef8_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef9_WORD	13
#define SCALER_X_REG__b_m_nscl_hor_coef9_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef10_WORD	14
#define SCALER_X_REG__b_m_nscl_hor_coef10_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef11_WORD	14
#define SCALER_X_REG__b_m_nscl_hor_coef11_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef12_WORD	15
#define SCALER_X_REG__b_m_nscl_hor_coef12_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef13_WORD	15
#define SCALER_X_REG__b_m_nscl_hor_coef13_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef14_WORD	16
#define SCALER_X_REG__b_m_nscl_hor_coef14_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef15_WORD	16
#define SCALER_X_REG__b_m_nscl_hor_coef15_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef16_WORD	17
#define SCALER_X_REG__b_m_nscl_hor_coef16_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef17_WORD	17
#define SCALER_X_REG__b_m_nscl_hor_coef17_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef18_WORD	18
#define SCALER_X_REG__b_m_nscl_hor_coef18_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef19_WORD	18
#define SCALER_X_REG__b_m_nscl_hor_coef19_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef20_WORD	19
#define SCALER_X_REG__b_m_nscl_hor_coef20_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef21_WORD	19
#define SCALER_X_REG__b_m_nscl_hor_coef21_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef22_WORD	20
#define SCALER_X_REG__b_m_nscl_hor_coef22_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef23_WORD	20
#define SCALER_X_REG__b_m_nscl_hor_coef23_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef24_WORD	21
#define SCALER_X_REG__b_m_nscl_hor_coef24_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef25_WORD	21
#define SCALER_X_REG__b_m_nscl_hor_coef25_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef26_WORD	22
#define SCALER_X_REG__b_m_nscl_hor_coef26_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef27_WORD	22
#define SCALER_X_REG__b_m_nscl_hor_coef27_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef28_WORD	23
#define SCALER_X_REG__b_m_nscl_hor_coef28_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef29_WORD	23
#define SCALER_X_REG__b_m_nscl_hor_coef29_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef30_WORD	24
#define SCALER_X_REG__b_m_nscl_hor_coef30_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef31_WORD	24
#define SCALER_X_REG__b_m_nscl_hor_coef31_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef32_WORD	25
#define SCALER_X_REG__b_m_nscl_hor_coef32_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef33_WORD	25
#define SCALER_X_REG__b_m_nscl_hor_coef33_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef34_WORD	26
#define SCALER_X_REG__b_m_nscl_hor_coef34_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef35_WORD	26
#define SCALER_X_REG__b_m_nscl_hor_coef35_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef36_WORD	27
#define SCALER_X_REG__b_m_nscl_hor_coef36_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef37_WORD	27
#define SCALER_X_REG__b_m_nscl_hor_coef37_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef38_WORD	28
#define SCALER_X_REG__b_m_nscl_hor_coef38_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef39_WORD	28
#define SCALER_X_REG__b_m_nscl_hor_coef39_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef40_WORD	29
#define SCALER_X_REG__b_m_nscl_hor_coef40_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef41_WORD	29
#define SCALER_X_REG__b_m_nscl_hor_coef41_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef42_WORD	30
#define SCALER_X_REG__b_m_nscl_hor_coef42_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef43_WORD	30
#define SCALER_X_REG__b_m_nscl_hor_coef43_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef44_WORD	31
#define SCALER_X_REG__b_m_nscl_hor_coef44_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef45_WORD	31
#define SCALER_X_REG__b_m_nscl_hor_coef45_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef46_WORD	32
#define SCALER_X_REG__b_m_nscl_hor_coef46_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef47_WORD	32
#define SCALER_X_REG__b_m_nscl_hor_coef47_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef48_WORD	33
#define SCALER_X_REG__b_m_nscl_hor_coef48_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef49_WORD	33
#define SCALER_X_REG__b_m_nscl_hor_coef49_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef50_WORD	34
#define SCALER_X_REG__b_m_nscl_hor_coef50_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef51_WORD	34
#define SCALER_X_REG__b_m_nscl_hor_coef51_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef52_WORD	35
#define SCALER_X_REG__b_m_nscl_hor_coef52_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef53_WORD	35
#define SCALER_X_REG__b_m_nscl_hor_coef53_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef54_WORD	36
#define SCALER_X_REG__b_m_nscl_hor_coef54_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef55_WORD	36
#define SCALER_X_REG__b_m_nscl_hor_coef55_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef56_WORD	37
#define SCALER_X_REG__b_m_nscl_hor_coef56_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef57_WORD	37
#define SCALER_X_REG__b_m_nscl_hor_coef57_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef58_WORD	38
#define SCALER_X_REG__b_m_nscl_hor_coef58_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef59_WORD	38
#define SCALER_X_REG__b_m_nscl_hor_coef59_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef60_WORD	39
#define SCALER_X_REG__b_m_nscl_hor_coef60_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef61_WORD	39
#define SCALER_X_REG__b_m_nscl_hor_coef61_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef62_WORD	40
#define SCALER_X_REG__b_m_nscl_hor_coef62_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef63_WORD	40
#define SCALER_X_REG__b_m_nscl_hor_coef63_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef64_WORD	41
#define SCALER_X_REG__b_m_nscl_hor_coef64_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef65_WORD	41
#define SCALER_X_REG__b_m_nscl_hor_coef65_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef66_WORD	42
#define SCALER_X_REG__b_m_nscl_hor_coef66_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef67_WORD	42
#define SCALER_X_REG__b_m_nscl_hor_coef67_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef68_WORD	43
#define SCALER_X_REG__b_m_nscl_hor_coef68_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef69_WORD	43
#define SCALER_X_REG__b_m_nscl_hor_coef69_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef70_WORD	44
#define SCALER_X_REG__b_m_nscl_hor_coef70_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef71_WORD	44
#define SCALER_X_REG__b_m_nscl_hor_coef71_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef72_WORD	45
#define SCALER_X_REG__b_m_nscl_hor_coef72_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef73_WORD	45
#define SCALER_X_REG__b_m_nscl_hor_coef73_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef74_WORD	46
#define SCALER_X_REG__b_m_nscl_hor_coef74_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef75_WORD	46
#define SCALER_X_REG__b_m_nscl_hor_coef75_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef76_WORD	47
#define SCALER_X_REG__b_m_nscl_hor_coef76_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef77_WORD	47
#define SCALER_X_REG__b_m_nscl_hor_coef77_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef78_WORD	48
#define SCALER_X_REG__b_m_nscl_hor_coef78_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef79_WORD	48
#define SCALER_X_REG__b_m_nscl_hor_coef79_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef80_WORD	49
#define SCALER_X_REG__b_m_nscl_hor_coef80_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef81_WORD	49
#define SCALER_X_REG__b_m_nscl_hor_coef81_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef82_WORD	50
#define SCALER_X_REG__b_m_nscl_hor_coef82_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef83_WORD	50
#define SCALER_X_REG__b_m_nscl_hor_coef83_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef84_WORD	51
#define SCALER_X_REG__b_m_nscl_hor_coef84_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef85_WORD	51
#define SCALER_X_REG__b_m_nscl_hor_coef85_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef86_WORD	52
#define SCALER_X_REG__b_m_nscl_hor_coef86_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef87_WORD	52
#define SCALER_X_REG__b_m_nscl_hor_coef87_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef88_WORD	53
#define SCALER_X_REG__b_m_nscl_hor_coef88_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef89_WORD	53
#define SCALER_X_REG__b_m_nscl_hor_coef89_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef90_WORD	54
#define SCALER_X_REG__b_m_nscl_hor_coef90_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef91_WORD	54
#define SCALER_X_REG__b_m_nscl_hor_coef91_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef92_WORD	55
#define SCALER_X_REG__b_m_nscl_hor_coef92_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef93_WORD	55
#define SCALER_X_REG__b_m_nscl_hor_coef93_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef94_WORD	56
#define SCALER_X_REG__b_m_nscl_hor_coef94_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef95_WORD	56
#define SCALER_X_REG__b_m_nscl_hor_coef95_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef96_WORD	57
#define SCALER_X_REG__b_m_nscl_hor_coef96_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef97_WORD	57
#define SCALER_X_REG__b_m_nscl_hor_coef97_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef98_WORD	58
#define SCALER_X_REG__b_m_nscl_hor_coef98_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef99_WORD	58
#define SCALER_X_REG__b_m_nscl_hor_coef99_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef100_WORD	59
#define SCALER_X_REG__b_m_nscl_hor_coef100_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef101_WORD	59
#define SCALER_X_REG__b_m_nscl_hor_coef101_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef102_WORD	60
#define SCALER_X_REG__b_m_nscl_hor_coef102_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef103_WORD	60
#define SCALER_X_REG__b_m_nscl_hor_coef103_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef104_WORD	61
#define SCALER_X_REG__b_m_nscl_hor_coef104_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef105_WORD	61
#define SCALER_X_REG__b_m_nscl_hor_coef105_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef106_WORD	62
#define SCALER_X_REG__b_m_nscl_hor_coef106_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef107_WORD	62
#define SCALER_X_REG__b_m_nscl_hor_coef107_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef108_WORD	63
#define SCALER_X_REG__b_m_nscl_hor_coef108_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef109_WORD	63
#define SCALER_X_REG__b_m_nscl_hor_coef109_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_hor_coef110_WORD	64
#define SCALER_X_REG__b_m_nscl_hor_coef110_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_hor_coef111_WORD	64
#define SCALER_X_REG__b_m_nscl_hor_coef111_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef0_WORD	65
#define SCALER_X_REG__b_m_nscl_ver_coef0_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef1_WORD	65
#define SCALER_X_REG__b_m_nscl_ver_coef1_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef2_WORD	66
#define SCALER_X_REG__b_m_nscl_ver_coef2_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef3_WORD	66
#define SCALER_X_REG__b_m_nscl_ver_coef3_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef4_WORD	67
#define SCALER_X_REG__b_m_nscl_ver_coef4_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef5_WORD	67
#define SCALER_X_REG__b_m_nscl_ver_coef5_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef6_WORD	68
#define SCALER_X_REG__b_m_nscl_ver_coef6_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef7_WORD	68
#define SCALER_X_REG__b_m_nscl_ver_coef7_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef8_WORD	69
#define SCALER_X_REG__b_m_nscl_ver_coef8_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef9_WORD	69
#define SCALER_X_REG__b_m_nscl_ver_coef9_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef10_WORD	70
#define SCALER_X_REG__b_m_nscl_ver_coef10_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef11_WORD	70
#define SCALER_X_REG__b_m_nscl_ver_coef11_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef12_WORD	71
#define SCALER_X_REG__b_m_nscl_ver_coef12_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef13_WORD	71
#define SCALER_X_REG__b_m_nscl_ver_coef13_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef14_WORD	72
#define SCALER_X_REG__b_m_nscl_ver_coef14_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef15_WORD	72
#define SCALER_X_REG__b_m_nscl_ver_coef15_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef16_WORD	73
#define SCALER_X_REG__b_m_nscl_ver_coef16_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef17_WORD	73
#define SCALER_X_REG__b_m_nscl_ver_coef17_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef18_WORD	74
#define SCALER_X_REG__b_m_nscl_ver_coef18_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef19_WORD	74
#define SCALER_X_REG__b_m_nscl_ver_coef19_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef20_WORD	75
#define SCALER_X_REG__b_m_nscl_ver_coef20_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef21_WORD	75
#define SCALER_X_REG__b_m_nscl_ver_coef21_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef22_WORD	76
#define SCALER_X_REG__b_m_nscl_ver_coef22_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef23_WORD	76
#define SCALER_X_REG__b_m_nscl_ver_coef23_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef24_WORD	77
#define SCALER_X_REG__b_m_nscl_ver_coef24_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef25_WORD	77
#define SCALER_X_REG__b_m_nscl_ver_coef25_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef26_WORD	78
#define SCALER_X_REG__b_m_nscl_ver_coef26_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef27_WORD	78
#define SCALER_X_REG__b_m_nscl_ver_coef27_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef28_WORD	79
#define SCALER_X_REG__b_m_nscl_ver_coef28_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef29_WORD	79
#define SCALER_X_REG__b_m_nscl_ver_coef29_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef30_WORD	80
#define SCALER_X_REG__b_m_nscl_ver_coef30_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef31_WORD	80
#define SCALER_X_REG__b_m_nscl_ver_coef31_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef32_WORD	81
#define SCALER_X_REG__b_m_nscl_ver_coef32_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef33_WORD	81
#define SCALER_X_REG__b_m_nscl_ver_coef33_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef34_WORD	82
#define SCALER_X_REG__b_m_nscl_ver_coef34_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef35_WORD	82
#define SCALER_X_REG__b_m_nscl_ver_coef35_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef36_WORD	83
#define SCALER_X_REG__b_m_nscl_ver_coef36_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef37_WORD	83
#define SCALER_X_REG__b_m_nscl_ver_coef37_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef38_WORD	84
#define SCALER_X_REG__b_m_nscl_ver_coef38_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef39_WORD	84
#define SCALER_X_REG__b_m_nscl_ver_coef39_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef40_WORD	85
#define SCALER_X_REG__b_m_nscl_ver_coef40_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef41_WORD	85
#define SCALER_X_REG__b_m_nscl_ver_coef41_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef42_WORD	86
#define SCALER_X_REG__b_m_nscl_ver_coef42_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef43_WORD	86
#define SCALER_X_REG__b_m_nscl_ver_coef43_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef44_WORD	87
#define SCALER_X_REG__b_m_nscl_ver_coef44_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef45_WORD	87
#define SCALER_X_REG__b_m_nscl_ver_coef45_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_m_nscl_ver_coef46_WORD	88
#define SCALER_X_REG__b_m_nscl_ver_coef46_MASK	GENMASK(15, 0)
#define SCALER_X_REG__b_m_nscl_ver_coef47_WORD	88
#define SCALER_X_REG__b_m_nscl_ver_coef47_MASK	GENMASK(31, 16)
#define SCALER_X_REG__b_cfg_se_WORD	121
#define SCALER_X_REG__b_cfg_se_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_force_update_en_WORD	122
#define SCALER_X_REG__b_force_update_en_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_vsync_update_en_WORD	122
#define SCALER_X_REG__b_vsync_update_en_MASK	GENMASK(1, 1)
#define SCALER_X_REG__b_shadow_read_en_WORD	122
#define SCALER_X_REG__b_shadow_read_en_MASK	GENMASK(2, 2)
#define SCALER_X_REG__b_force_update_pulse_WORD	123
#define SCALER_X_REG__b_force_update_pulse_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_force_update_en_se_WORD	124
#define SCALER_X_REG__b_force_update_en_se_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_vsync_update_en_se_WORD	124
#define SCALER_X_REG__b_vsync_update_en_se_MASK	GENMASK(1, 1)
#define SCALER_X_REG__b_shadow_read_en_se_WORD	124
#define SCALER_X_REG__b_shadow_read_en_se_MASK	GENMASK(2, 2)
#define SCALER_X_REG__b_force_update_pulse_se_WORD	125
#define SCALER_X_REG__b_force_update_pulse_se_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_icg_override_WORD	126
#define SCALER_X_REG__b_icg_override_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_trigger_WORD	127
#define SCALER_X_REG__b_trigger_MASK	GENMASK(0, 0)
#define SCALER_X_REG__b_trigger2_WORD	128
#define SCALER_X_REG__b_trigger2_MASK	GENMASK(0, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_0_WORD	0
#define SCALER_X_REG__v_saturn_scaler_reg_0_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_1_WORD	1
#define SCALER_X_REG__v_saturn_scaler_reg_1_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_2_WORD	2
#define SCALER_X_REG__v_saturn_scaler_reg_2_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_3_WORD	3
#define SCALER_X_REG__v_saturn_scaler_reg_3_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_4_WORD	4
#define SCALER_X_REG__v_saturn_scaler_reg_4_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_5_WORD	5
#define SCALER_X_REG__v_saturn_scaler_reg_5_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_6_WORD	6
#define SCALER_X_REG__v_saturn_scaler_reg_6_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_7_WORD	7
#define SCALER_X_REG__v_saturn_scaler_reg_7_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_8_WORD	8
#define SCALER_X_REG__v_saturn_scaler_reg_8_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_9_WORD	9
#define SCALER_X_REG__v_saturn_scaler_reg_9_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_10_WORD	10
#define SCALER_X_REG__v_saturn_scaler_reg_10_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_11_WORD	11
#define SCALER_X_REG__v_saturn_scaler_reg_11_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_12_WORD	12
#define SCALER_X_REG__v_saturn_scaler_reg_12_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_13_WORD	13
#define SCALER_X_REG__v_saturn_scaler_reg_13_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_14_WORD	14
#define SCALER_X_REG__v_saturn_scaler_reg_14_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_15_WORD	15
#define SCALER_X_REG__v_saturn_scaler_reg_15_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_16_WORD	16
#define SCALER_X_REG__v_saturn_scaler_reg_16_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_17_WORD	17
#define SCALER_X_REG__v_saturn_scaler_reg_17_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_18_WORD	18
#define SCALER_X_REG__v_saturn_scaler_reg_18_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_19_WORD	19
#define SCALER_X_REG__v_saturn_scaler_reg_19_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_20_WORD	20
#define SCALER_X_REG__v_saturn_scaler_reg_20_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_21_WORD	21
#define SCALER_X_REG__v_saturn_scaler_reg_21_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_22_WORD	22
#define SCALER_X_REG__v_saturn_scaler_reg_22_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_23_WORD	23
#define SCALER_X_REG__v_saturn_scaler_reg_23_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_24_WORD	24
#define SCALER_X_REG__v_saturn_scaler_reg_24_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_25_WORD	25
#define SCALER_X_REG__v_saturn_scaler_reg_25_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_26_WORD	26
#define SCALER_X_REG__v_saturn_scaler_reg_26_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_27_WORD	27
#define SCALER_X_REG__v_saturn_scaler_reg_27_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_28_WORD	28
#define SCALER_X_REG__v_saturn_scaler_reg_28_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_29_WORD	29
#define SCALER_X_REG__v_saturn_scaler_reg_29_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_30_WORD	30
#define SCALER_X_REG__v_saturn_scaler_reg_30_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_31_WORD	31
#define SCALER_X_REG__v_saturn_scaler_reg_31_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_32_WORD	32
#define SCALER_X_REG__v_saturn_scaler_reg_32_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_33_WORD	33
#define SCALER_X_REG__v_saturn_scaler_reg_33_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_34_WORD	34
#define SCALER_X_REG__v_saturn_scaler_reg_34_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_35_WORD	35
#define SCALER_X_REG__v_saturn_scaler_reg_35_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_36_WORD	36
#define SCALER_X_REG__v_saturn_scaler_reg_36_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_37_WORD	37
#define SCALER_X_REG__v_saturn_scaler_reg_37_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_38_WORD	38
#define SCALER_X_REG__v_saturn_scaler_reg_38_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_39_WORD	39
#define SCALER_X_REG__v_saturn_scaler_reg_39_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_40_WORD	40
#define SCALER_X_REG__v_saturn_scaler_reg_40_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_41_WORD	41
#define SCALER_X_REG__v_saturn_scaler_reg_41_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_42_WORD	42
#define SCALER_X_REG__v_saturn_scaler_reg_42_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_43_WORD	43
#define SCALER_X_REG__v_saturn_scaler_reg_43_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_44_WORD	44
#define SCALER_X_REG__v_saturn_scaler_reg_44_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_45_WORD	45
#define SCALER_X_REG__v_saturn_scaler_reg_45_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_46_WORD	46
#define SCALER_X_REG__v_saturn_scaler_reg_46_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_47_WORD	47
#define SCALER_X_REG__v_saturn_scaler_reg_47_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_48_WORD	48
#define SCALER_X_REG__v_saturn_scaler_reg_48_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_49_WORD	49
#define SCALER_X_REG__v_saturn_scaler_reg_49_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_50_WORD	50
#define SCALER_X_REG__v_saturn_scaler_reg_50_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_51_WORD	51
#define SCALER_X_REG__v_saturn_scaler_reg_51_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_52_WORD	52
#define SCALER_X_REG__v_saturn_scaler_reg_52_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_53_WORD	53
#define SCALER_X_REG__v_saturn_scaler_reg_53_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_54_WORD	54
#define SCALER_X_REG__v_saturn_scaler_reg_54_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_55_WORD	55
#define SCALER_X_REG__v_saturn_scaler_reg_55_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_56_WORD	56
#define SCALER_X_REG__v_saturn_scaler_reg_56_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_57_WORD	57
#define SCALER_X_REG__v_saturn_scaler_reg_57_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_58_WORD	58
#define SCALER_X_REG__v_saturn_scaler_reg_58_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_59_WORD	59
#define SCALER_X_REG__v_saturn_scaler_reg_59_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_60_WORD	60
#define SCALER_X_REG__v_saturn_scaler_reg_60_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_61_WORD	61
#define SCALER_X_REG__v_saturn_scaler_reg_61_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_62_WORD	62
#define SCALER_X_REG__v_saturn_scaler_reg_62_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_63_WORD	63
#define SCALER_X_REG__v_saturn_scaler_reg_63_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_64_WORD	64
#define SCALER_X_REG__v_saturn_scaler_reg_64_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_65_WORD	65
#define SCALER_X_REG__v_saturn_scaler_reg_65_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_66_WORD	66
#define SCALER_X_REG__v_saturn_scaler_reg_66_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_67_WORD	67
#define SCALER_X_REG__v_saturn_scaler_reg_67_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_68_WORD	68
#define SCALER_X_REG__v_saturn_scaler_reg_68_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_69_WORD	69
#define SCALER_X_REG__v_saturn_scaler_reg_69_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_70_WORD	70
#define SCALER_X_REG__v_saturn_scaler_reg_70_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_71_WORD	71
#define SCALER_X_REG__v_saturn_scaler_reg_71_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_72_WORD	72
#define SCALER_X_REG__v_saturn_scaler_reg_72_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_73_WORD	73
#define SCALER_X_REG__v_saturn_scaler_reg_73_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_74_WORD	74
#define SCALER_X_REG__v_saturn_scaler_reg_74_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_75_WORD	75
#define SCALER_X_REG__v_saturn_scaler_reg_75_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_76_WORD	76
#define SCALER_X_REG__v_saturn_scaler_reg_76_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_77_WORD	77
#define SCALER_X_REG__v_saturn_scaler_reg_77_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_78_WORD	78
#define SCALER_X_REG__v_saturn_scaler_reg_78_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_79_WORD	79
#define SCALER_X_REG__v_saturn_scaler_reg_79_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_80_WORD	80
#define SCALER_X_REG__v_saturn_scaler_reg_80_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_81_WORD	81
#define SCALER_X_REG__v_saturn_scaler_reg_81_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_82_WORD	82
#define SCALER_X_REG__v_saturn_scaler_reg_82_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_83_WORD	83
#define SCALER_X_REG__v_saturn_scaler_reg_83_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_84_WORD	84
#define SCALER_X_REG__v_saturn_scaler_reg_84_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_85_WORD	85
#define SCALER_X_REG__v_saturn_scaler_reg_85_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_86_WORD	86
#define SCALER_X_REG__v_saturn_scaler_reg_86_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_87_WORD	87
#define SCALER_X_REG__v_saturn_scaler_reg_87_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_88_WORD	88
#define SCALER_X_REG__v_saturn_scaler_reg_88_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_89_WORD	89
#define SCALER_X_REG__v_saturn_scaler_reg_89_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_90_WORD	90
#define SCALER_X_REG__v_saturn_scaler_reg_90_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_91_WORD	91
#define SCALER_X_REG__v_saturn_scaler_reg_91_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_92_WORD	92
#define SCALER_X_REG__v_saturn_scaler_reg_92_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_93_WORD	93
#define SCALER_X_REG__v_saturn_scaler_reg_93_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_94_WORD	94
#define SCALER_X_REG__v_saturn_scaler_reg_94_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_95_WORD	95
#define SCALER_X_REG__v_saturn_scaler_reg_95_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_96_WORD	96
#define SCALER_X_REG__v_saturn_scaler_reg_96_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_97_WORD	97
#define SCALER_X_REG__v_saturn_scaler_reg_97_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_98_WORD	98
#define SCALER_X_REG__v_saturn_scaler_reg_98_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_99_WORD	99
#define SCALER_X_REG__v_saturn_scaler_reg_99_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_100_WORD	100
#define SCALER_X_REG__v_saturn_scaler_reg_100_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_101_WORD	101
#define SCALER_X_REG__v_saturn_scaler_reg_101_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_102_WORD	102
#define SCALER_X_REG__v_saturn_scaler_reg_102_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_103_WORD	103
#define SCALER_X_REG__v_saturn_scaler_reg_103_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_104_WORD	104
#define SCALER_X_REG__v_saturn_scaler_reg_104_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_105_WORD	105
#define SCALER_X_REG__v_saturn_scaler_reg_105_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_106_WORD	106
#define SCALER_X_REG__v_saturn_scaler_reg_106_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_107_WORD	107
#define SCALER_X_REG__v_saturn_scaler_reg_107_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_108_WORD	108
#define SCALER_X_REG__v_saturn_scaler_reg_108_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_109_WORD	109
#define SCALER_X_REG__v_saturn_scaler_reg_109_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_110_WORD	110
#define SCALER_X_REG__v_saturn_scaler_reg_110_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_111_WORD	111
#define SCALER_X_REG__v_saturn_scaler_reg_111_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_112_WORD	112
#define SCALER_X_REG__v_saturn_scaler_reg_112_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_113_WORD	113
#define SCALER_X_REG__v_saturn_scaler_reg_113_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_114_WORD	114
#define SCALER_X_REG__v_saturn_scaler_reg_114_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_115_WORD	115
#define SCALER_X_REG__v_saturn_scaler_reg_115_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_116_WORD	116
#define SCALER_X_REG__v_saturn_scaler_reg_116_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_117_WORD	117
#define SCALER_X_REG__v_saturn_scaler_reg_117_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_118_WORD	118
#define SCALER_X_REG__v_saturn_scaler_reg_118_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_119_WORD	119
#define SCALER_X_REG__v_saturn_scaler_reg_119_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_120_WORD	120
#define SCALER_X_REG__v_saturn_scaler_reg_120_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_121_WORD	121
#define SCALER_X_REG__v_saturn_scaler_reg_121_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_122_WORD	122
#define SCALER_X_REG__v_saturn_scaler_reg_122_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_123_WORD	123
#define SCALER_X_REG__v_saturn_scaler_reg_123_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_124_WORD	124
#define SCALER_X_REG__v_saturn_scaler_reg_124_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_125_WORD	125
#define SCALER_X_REG__v_saturn_scaler_reg_125_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_126_WORD	126
#define SCALER_X_REG__v_saturn_scaler_reg_126_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_127_WORD	127
#define SCALER_X_REG__v_saturn_scaler_reg_127_MASK	GENMASK(31, 0)
#define SCALER_X_REG__v_saturn_scaler_reg_128_WORD	128
#define SCALER_X_REG__v_saturn_scaler_reg_128_MASK	GENMASK(31, 0)

/* TMG_REG (tmg.h): 30 x 32-bit registers */
#define TMG_REG__NWORDS	30
#define TMG_REG__split_en_WORD	0
#define TMG_REG__split_en_MASK	GENMASK(0, 0)
#define TMG_REG__cmd_screen_WORD	0
#define TMG_REG__cmd_screen_MASK	GENMASK(1, 1)
#define TMG_REG__fm_timing_en_WORD	0
#define TMG_REG__fm_timing_en_MASK	GENMASK(2, 2)
#define TMG_REG__cmd_wait_en_WORD	0
#define TMG_REG__cmd_wait_en_MASK	GENMASK(3, 3)
#define TMG_REG__cmd_wait_te_WORD	0
#define TMG_REG__cmd_wait_te_MASK	GENMASK(4, 4)
#define TMG_REG__disp_ready_man_en_WORD	0
#define TMG_REG__disp_ready_man_en_MASK	GENMASK(5, 5)
#define TMG_REG__ready_1_non_active_WORD	0
#define TMG_REG__ready_1_non_active_MASK	GENMASK(6, 6)
#define TMG_REG__hsp_WORD	0
#define TMG_REG__hsp_MASK	GENMASK(7, 7)
#define TMG_REG__vsp_WORD	0
#define TMG_REG__vsp_MASK	GENMASK(8, 8)
#define TMG_REG__fm_cmd_tmout_eq_eof_WORD	0
#define TMG_REG__fm_cmd_tmout_eq_eof_MASK	GENMASK(9, 9)
#define TMG_REG__skip_frm_en_WORD	0
#define TMG_REG__skip_frm_en_MASK	GENMASK(10, 10)
#define TMG_REG__swap_rgb_en_WORD	0
#define TMG_REG__swap_rgb_en_MASK	GENMASK(11, 11)
#define TMG_REG__dsc_enable_WORD	1
#define TMG_REG__dsc_enable_MASK	GENMASK(0, 0)
#define TMG_REG__dsc_one_line_byte_remainder_WORD	1
#define TMG_REG__dsc_one_line_byte_remainder_MASK	GENMASK(2, 1)
#define TMG_REG__drift_timeout_WORD	2
#define TMG_REG__drift_timeout_MASK	GENMASK(11, 0)
#define TMG_REG__user_WORD	2
#define TMG_REG__user_MASK	GENMASK(15, 12)
#define TMG_REG__active_frm_num_WORD	2
#define TMG_REG__active_frm_num_MASK	GENMASK(23, 16)
#define TMG_REG__inactive_frm_num_WORD	2
#define TMG_REG__inactive_frm_num_MASK	GENMASK(31, 24)
#define TMG_REG__split_overlap_WORD	3
#define TMG_REG__split_overlap_MASK	GENMASK(9, 0)
#define TMG_REG__hfp_WORD	3
#define TMG_REG__hfp_MASK	GENMASK(27, 16)
#define TMG_REG__hsync_width_WORD	4
#define TMG_REG__hsync_width_MASK	GENMASK(9, 0)
#define TMG_REG__hbp_WORD	4
#define TMG_REG__hbp_MASK	GENMASK(27, 16)
#define TMG_REG__vfp_WORD	5
#define TMG_REG__vfp_MASK	GENMASK(15, 0)
#define TMG_REG__vsync_width_WORD	5
#define TMG_REG__vsync_width_MASK	GENMASK(25, 16)
#define TMG_REG__vbp_WORD	6
#define TMG_REG__vbp_MASK	GENMASK(11, 0)
#define TMG_REG__h_active_WORD	6
#define TMG_REG__h_active_MASK	GENMASK(29, 16)
#define TMG_REG__v_active_WORD	7
#define TMG_REG__v_active_MASK	GENMASK(13, 0)
#define TMG_REG__fm_cmd_tmout_num_WORD	8
#define TMG_REG__fm_cmd_tmout_num_MASK	GENMASK(23, 0)
#define TMG_REG__background_r_WORD	9
#define TMG_REG__background_r_MASK	GENMASK(11, 0)
#define TMG_REG__background_g_WORD	9
#define TMG_REG__background_g_MASK	GENMASK(27, 16)
#define TMG_REG__background_b_WORD	10
#define TMG_REG__background_b_MASK	GENMASK(11, 0)
#define TMG_REG__eof_1st_ln_dly_num_WORD	11
#define TMG_REG__eof_1st_ln_dly_num_MASK	GENMASK(15, 0)
#define TMG_REG__eof_2nd_ln_dly_num_WORD	11
#define TMG_REG__eof_2nd_ln_dly_num_MASK	GENMASK(31, 16)
#define TMG_REG__cfg_ln_num_intp_WORD	12
#define TMG_REG__cfg_ln_num_intp_MASK	GENMASK(16, 0)
#define TMG_REG__frame_te_dly_num_WORD	13
#define TMG_REG__frame_te_dly_num_MASK	GENMASK(31, 0)
#define TMG_REG__sof_pre_ln_num_WORD	14
#define TMG_REG__sof_pre_ln_num_MASK	GENMASK(16, 0)
#define TMG_REG__vsync_align_with_hsync_WORD	15
#define TMG_REG__vsync_align_with_hsync_MASK	GENMASK(0, 0)
#define TMG_REG__dfc_low_thre_WORD	16
#define TMG_REG__dfc_low_thre_MASK	GENMASK(11, 0)
#define TMG_REG__dfc_high_thre_WORD	16
#define TMG_REG__dfc_high_thre_MASK	GENMASK(27, 16)
#define TMG_REG__urgent_vld_en_WORD	17
#define TMG_REG__urgent_vld_en_MASK	GENMASK(0, 0)
#define TMG_REG__urgent_l_thre_WORD	17
#define TMG_REG__urgent_l_thre_MASK	GENMASK(19, 8)
#define TMG_REG__urgent_h_thre_WORD	18
#define TMG_REG__urgent_h_thre_MASK	GENMASK(11, 0)
#define TMG_REG__dbg_disp_ready_i_WORD	19
#define TMG_REG__dbg_disp_ready_i_MASK	GENMASK(1, 0)
#define TMG_REG__dbg_tmg_cur_state_i_WORD	20
#define TMG_REG__dbg_tmg_cur_state_i_MASK	GENMASK(31, 0)
#define TMG_REG__dfc_low_hold_cycle_WORD	21
#define TMG_REG__dfc_low_hold_cycle_MASK	GENMASK(31, 0)
#define TMG_REG__dbg_dfc_mode_WORD	22
#define TMG_REG__dbg_dfc_mode_MASK	GENMASK(0, 0)
#define TMG_REG__force_update_en_WORD	23
#define TMG_REG__force_update_en_MASK	GENMASK(0, 0)
#define TMG_REG__vsync_update_en_WORD	23
#define TMG_REG__vsync_update_en_MASK	GENMASK(1, 1)
#define TMG_REG__shadow_read_en_WORD	23
#define TMG_REG__shadow_read_en_MASK	GENMASK(2, 2)
#define TMG_REG__force_update_pulse_WORD	24
#define TMG_REG__force_update_pulse_MASK	GENMASK(0, 0)
#define TMG_REG__icg_override_WORD	25
#define TMG_REG__icg_override_MASK	GENMASK(0, 0)
#define TMG_REG__trigger_WORD	26
#define TMG_REG__trigger_MASK	GENMASK(0, 0)
#define TMG_REG__trigger2_WORD	27
#define TMG_REG__trigger2_MASK	GENMASK(0, 0)
#define TMG_REG__mem0_wr_cnt_WORD	28
#define TMG_REG__mem0_wr_cnt_MASK	GENMASK(31, 0)
#define TMG_REG__mem1_wr_cnt_WORD	29
#define TMG_REG__mem1_wr_cnt_MASK	GENMASK(31, 0)

#endif /* SATURN_FIELDS_H */
