//
//  parset.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/15.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "parset.h"
#include "frame.h"

// 因为sps_id的取值范围为[0,31]，因此数组容量最大为32，详见7.4.2.1
sps_t Sequence_Parameters_Set_Array[32];

#pragma mark - 函数声明
void parse_sps_syntax_element(sps_t *sps, bs_t *b);
void save_sps_as_available(sps_t *sps);
void scaling_list(int *scalingList, int sizeOfScalingList, int *useDefaultScalingMatrixFlag, bs_t *b);
void parse_vui_parameters(sps_t *sps, bs_t *b);
void parse_vui_hrd_parameters(hrd_parameters_t *hrd, bs_t *b);

#pragma mark - 函数实现
#pragma mark 解析sps句法元素

/**
 处理SPS，包含两步：
 先解析、后保存
 */
void processSPS(bs_t *b)
{
    sps_t *sps = allocSPS();
    // 0.解析
    parse_sps_syntax_element(sps, b);
    // 1.保存
    save_sps_as_available(sps);
    
    freeSPS(sps);
}

/**
 解析sps句法元素
 [h264协议文档位置]：7.3.2.1.1 Sequence parameter set data syntax
 */
void parse_sps_syntax_element(sps_t *sps, bs_t *b)
{
    sps->profile_idc = bs_read_u(b, 8);
    sps->constraint_set0_flag = bs_read_u(b, 1);
    sps->constraint_set1_flag = bs_read_u(b, 1);
    sps->constraint_set2_flag = bs_read_u(b, 1);
    sps->constraint_set3_flag = bs_read_u(b, 1);
    sps->constraint_set4_flag = bs_read_u(b, 1);
    sps->constraint_set5_flag = bs_read_u(b, 1);
    sps->reserved_zero_2bits = bs_read_u(b, 2);
    sps->level_idc = bs_read_u(b, 8);
    
    sps->seq_parameter_set_id = bs_read_ue(b);
    
    if (sps->profile_idc == 100 || sps->profile_idc == 110 || sps->profile_idc == 122 || sps->profile_idc == 244 || sps->profile_idc == 44 || sps->profile_idc == 83 || sps->profile_idc == 86 || sps->profile_idc == 118 || sps->profile_idc == 128 || sps->profile_idc == 138 || sps->profile_idc == 139 || sps->profile_idc == 134 || sps->profile_idc == 135) {
        
        sps->chroma_format_idc = bs_read_ue(b);
        if (sps->chroma_format_idc == YUV_4_4_4) {
            sps->separate_colour_plane_flag = bs_read_u(b, 1);
        }
        sps->bit_depth_luma_minus8 = bs_read_ue(b);
        sps->bit_depth_chroma_minus8 = bs_read_ue(b);
        sps->qpprime_y_zero_transform_bypass_flag = bs_read_u(b, 1);
        sps->seq_scaling_matrix_present_flag = bs_read_u(b, 1);
        
        if (sps->seq_scaling_matrix_present_flag) {
            int scalingListCycle = (sps->chroma_format_idc != YUV_4_4_4) ? 8 : 12;
            for (int i = 0; i < scalingListCycle; i++) {
                sps->seq_scaling_list_present_flag[i] = bs_read_u(b, 1);
                if (sps->seq_scaling_list_present_flag[i]) {
                    if (i < 6) {
                        scaling_list(sps->ScalingList4x4[i], 16, &sps->UseDefaultScalingMatrix4x4Flag[i], b);
                    }else {
                        scaling_list(sps->ScalingList8x8[i-6], 64, &sps->UseDefaultScalingMatrix8x8Flag[i-6], b);
                    }
                }
            }
        }
    }
    
    sps->log2_max_frame_num_minus4 = bs_read_ue(b);
    sps->pic_order_cnt_type = bs_read_ue(b);
    if (sps->pic_order_cnt_type == 0) {
        sps->log2_max_pic_order_cnt_lsb_minus4 = bs_read_ue(b);
    }else if (sps->pic_order_cnt_type == 1) {
        sps->delta_pic_order_always_zero_flag = bs_read_u(b, 1);
        sps->offset_for_non_ref_pic = bs_read_se(b);
        sps->offset_for_top_to_bottom_field = bs_read_se(b);
        sps->num_ref_frames_in_pic_order_cnt_cycle = bs_read_ue(b);
        for (int i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
            sps->offset_for_ref_frame[i] = bs_read_se(b);
        }
    }
    
    sps->max_num_ref_frames = bs_read_ue(b);
    sps->gaps_in_frame_num_value_allowed_flag = bs_read_u(b, 1);
    
    sps->pic_width_in_mbs_minus1 = bs_read_ue(b);
    sps->pic_height_in_map_units_minus1 = bs_read_ue(b);
    sps->frame_mbs_only_flag = bs_read_u(b, 1);
    if (!sps->frame_mbs_only_flag) {
        sps->mb_adaptive_frame_field_flag = bs_read_u(b, 1);
    }
    
    sps->direct_8x8_inference_flag = bs_read_u(b, 1);
    
    sps->frame_cropping_flag = bs_read_u(b, 1);
    if (sps->frame_cropping_flag) {
        sps->frame_crop_left_offset = bs_read_ue(b);
        sps->frame_crop_right_offset = bs_read_ue(b);
        sps->frame_crop_top_offset = bs_read_ue(b);
        sps->frame_crop_bottom_offset = bs_read_ue(b);
    }
    
    sps->vui_parameters_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters_present_flag) {
        parse_vui_parameters(sps, b);
    }
}

/**
 scaling_list函数实现
 [h264协议文档位置]：7.3.2.1.1 Scaling list syntax
 */
void scaling_list(int *scalingList, int sizeOfScalingList, int *useDefaultScalingMatrixFlag, bs_t *b)
{
    int deltaScale;
    int lastScale = 8;
    int nextScale = 8;
    
    for (int j = 0; j < sizeOfScalingList; j++) {
        
        if (nextScale != 0) {
            deltaScale = bs_read_se(b);
            nextScale = (lastScale + deltaScale + 256) % 256;
            *useDefaultScalingMatrixFlag = (j == 0 && nextScale == 0);
        }
        
        scalingList[j] = (nextScale == 0) ? lastScale : nextScale;
        lastScale = scalingList[j];
    }
}

/**
 解析vui_parameters()句法元素
 [h264协议文档位置]：Annex E.1.1
 */
void parse_vui_parameters(sps_t *sps, bs_t *b)
{
    sps->vui_parameters.aspect_ratio_info_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.aspect_ratio_info_present_flag) {
        sps->vui_parameters.aspect_ratio_idc = bs_read_u(b, 8);
        if (sps->vui_parameters.aspect_ratio_idc == 255) { // Extended_SAR值为255
            sps->vui_parameters.sar_width = bs_read_u(b, 16);
            sps->vui_parameters.sar_height = bs_read_u(b, 16);
        }
    }
    
    sps->vui_parameters.overscan_info_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.overscan_info_present_flag) {
        sps->vui_parameters.overscan_appropriate_flag = bs_read_u(b, 1);
    }
    
    sps->vui_parameters.video_signal_type_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.video_signal_type_present_flag) {
        sps->vui_parameters.video_format = bs_read_u(b, 3);
        sps->vui_parameters.video_full_range_flag = bs_read_u(b, 1);
        
        sps->vui_parameters.colour_description_present_flag = bs_read_u(b, 1);
        if (sps->vui_parameters.colour_description_present_flag) {
            sps->vui_parameters.colour_primaries = bs_read_u(b, 8);
            sps->vui_parameters.transfer_characteristics = bs_read_u(b, 8);
            sps->vui_parameters.matrix_coefficients = bs_read_u(b, 8);
        }
    }
    
    sps->vui_parameters.chroma_loc_info_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.chroma_loc_info_present_flag) {
        sps->vui_parameters.chroma_sample_loc_type_top_field = bs_read_ue(b);
        sps->vui_parameters.chroma_sample_loc_type_bottom_field = bs_read_ue(b);
    }
    
    sps->vui_parameters.timing_info_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.timing_info_present_flag) {
        sps->vui_parameters.num_units_in_tick = bs_read_u(b, 32);
        sps->vui_parameters.time_scale = bs_read_u(b, 32);
        sps->vui_parameters.fixed_frame_rate_flag = bs_read_u(b, 1);
    }
    
    sps->vui_parameters.nal_hrd_parameters_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.nal_hrd_parameters_present_flag) {
        parse_vui_hrd_parameters(&sps->vui_parameters.nal_hrd_parameters, b);
    }
    
    sps->vui_parameters.vcl_hrd_parameters_present_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.vcl_hrd_parameters_present_flag) {
        parse_vui_hrd_parameters(&sps->vui_parameters.vcl_hrd_parameters, b);
    }
    
    if (sps->vui_parameters.nal_hrd_parameters_present_flag ||
        sps->vui_parameters.vcl_hrd_parameters_present_flag) {
        sps->vui_parameters.low_delay_hrd_flag = bs_read_u(b, 1);
    }
    
    sps->vui_parameters.pic_struct_present_flag = bs_read_u(b, 1);
    sps->vui_parameters.bitstream_restriction_flag = bs_read_u(b, 1);
    if (sps->vui_parameters.bitstream_restriction_flag) {
        sps->vui_parameters.motion_vectors_over_pic_boundaries_flag = bs_read_u(b, 1);
        sps->vui_parameters.max_bytes_per_pic_denom = bs_read_ue(b);
        sps->vui_parameters.max_bits_per_mb_denom = bs_read_ue(b);
        sps->vui_parameters.log2_max_mv_length_horizontal = bs_read_ue(b);
        sps->vui_parameters.log2_max_mv_length_vertical = bs_read_ue(b);
        sps->vui_parameters.max_num_reorder_frames = bs_read_ue(b);
        sps->vui_parameters.max_dec_frame_buffering = bs_read_ue(b);
    }
}

/**
 解析hrd_parameters()句法元素
 [h264协议文档位置]：Annex E.1.2
 */
void parse_vui_hrd_parameters(hrd_parameters_t *hrd, bs_t *b)
{
    hrd->cpb_cnt_minus1 = bs_read_ue(b);
    hrd->bit_rate_scale = bs_read_u(b, 4);
    hrd->cpb_size_scale = bs_read_u(b, 4);
    
    for (int SchedSelIdx = 0; SchedSelIdx <= hrd->cpb_cnt_minus1; SchedSelIdx++) {
        hrd->bit_rate_value_minus1[SchedSelIdx] = bs_read_ue(b);
        hrd->cpb_size_value_minus1[SchedSelIdx] = bs_read_ue(b);
        hrd->cbr_flag[SchedSelIdx] = bs_read_u(b, 1);
    }
    
    hrd->initial_cpb_removal_delay_length_minus1 = bs_read_u(b, 5);
    hrd->cpb_removal_delay_length_minus1 = bs_read_u(b, 5);
    hrd->dpb_output_delay_length_minus1 = bs_read_u(b, 5);
    hrd->time_offset_length = bs_read_u(b, 5);
}

void save_sps_as_available(sps_t *sps)
{
    memcpy (&Sequence_Parameters_Set_Array[sps->seq_parameter_set_id], sps, sizeof (sps_t));
}

// 初始化sps结构体
sps_t *allocSPS(void)
{
    sps_t *sps = calloc(1, sizeof(sps_t));
    if (sps == NULL) {
        fprintf(stderr, "%s\n", "Alloc SPS Error");
        exit(-1);
    }
    return sps;
}

// 释放sps
void freeSPS(sps_t *sps)
{
    free(sps);
}
