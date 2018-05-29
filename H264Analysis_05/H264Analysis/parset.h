//
//  parset.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/15.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//  序列参数集SPS和图像参数集PPS相关

#ifndef parset_h
#define parset_h

#include <stdio.h>
#include "bs.h"

#define MAX_CPB_CNT   32

/**
 hrd_parameters()
 @see Annex E E.1.2 HRD parameters syntax
 */
typedef struct {
    int cpb_cnt_minus1;                                 // ue(v)
    int bit_rate_scale;                                 // u(4)
    int cpb_size_scale;                                 // u(4)
    
//  for( SchedSelIdx = 0; SchedSelIdx <= cpb_cnt_minus1; SchedSelIdx++ ) {
        int bit_rate_value_minus1[MAX_CPB_CNT];         // ue(v)
        int cpb_size_value_minus1[MAX_CPB_CNT];         // ue(v)
        int cbr_flag[MAX_CPB_CNT];                      // u(1)
    
    int initial_cpb_removal_delay_length_minus1;        // u(5)
    int cpb_removal_delay_length_minus1;                // u(5)
    int dpb_output_delay_length_minus1;                 // u(5)
    int time_offset_length;                             // u(5)
}hrd_parameters_t;

/**
 vui_parameters()
 @see Annex E E.1.1 VUI parameters syntax
 */
typedef struct {
    int aspect_ratio_info_present_flag;                 // u(1)
//  if( aspect_ratio_info_present_flag ) {
        int aspect_ratio_idc;                           // u(8)
//      if( aspect_ratio_idc = = Extended_SAR ) {
            int sar_width;                              // u(16)
            int sar_height;                             // u(16)
    
    int overscan_info_present_flag;                     // u(1)
//  if( overscan_info_present_flag )
        int overscan_appropriate_flag;                  // u(1)
    int video_signal_type_present_flag;                 // u(1)
    
//  if( video_signal_type_present_flag ) {
        int video_format;                               // u(3)
        int video_full_range_flag;                      // u(1)
        int colour_description_present_flag;            // u(1)
//      if( colour_description_present_flag ) {
            int colour_primaries;                       // u(8)
            int transfer_characteristics;               // u(8)
            int matrix_coefficients;                    // u(8)
    
    int chroma_loc_info_present_flag;                   // u(1)
//  if( chroma_loc_info_present_flag ) {
        int chroma_sample_loc_type_top_field;           // ue(v)
        int chroma_sample_loc_type_bottom_field;        // ue(v)
    
    int timing_info_present_flag;                       // u(1)
//  if( timing_info_present_flag ) {
        int num_units_in_tick;                          // u(32)
        int time_scale;                                 // u(32)
        int fixed_frame_rate_flag;                      // u(1)
    
    int nal_hrd_parameters_present_flag;                // u(1)
//  if( nal_hrd_parameters_present_flag )
        hrd_parameters_t nal_hrd_parameters;
    
    int vcl_hrd_parameters_present_flag;                // u(1)
//  if( vcl_hrd_parameters_present_flag )
        hrd_parameters_t vcl_hrd_parameters;
//  if( nal_hrd_parameters_present_flag | | vcl_hrd_parameters_present_flag )
        int low_delay_hrd_flag;                         // u(1)
    
    int pic_struct_present_flag;                        // u(1)
    int bitstream_restriction_flag;                     // u(1)
//  if( bitstream_restriction_flag ) {
        int motion_vectors_over_pic_boundaries_flag;    // u(1)
        int max_bytes_per_pic_denom;                    // ue(v)
        int max_bits_per_mb_denom;                      // ue(v)
        int log2_max_mv_length_horizontal;              // ue(v)
        int log2_max_mv_length_vertical;                // ue(v)
        int max_num_reorder_frames;                     // ue(v)
        int max_dec_frame_buffering;                    // ue(v)
}vui_parameters_t;

/**
 Sequence Parameter Set
 @see 7.3.2.1 Sequence parameter set RBSP syntax
 */
typedef struct
{
    int profile_idc;                                    // u(8)
    /*  —————————— 编码级别的制约条件 Start  —————————— */
    int constraint_set0_flag;                           // u(1)
    int constraint_set1_flag;                           // u(1)
    int constraint_set2_flag;                           // u(1)
    int constraint_set3_flag;                           // u(1)
    int constraint_set4_flag;                           // u(1)
    int constraint_set5_flag;                           // u(1)
    int reserved_zero_2bits;                            // u(2)
    /*  —————————— 编码级别的制约条件 End  —————————— */
    
    int level_idc;                                      // u(8)
    int seq_parameter_set_id;                           // ue(v)
    
    /*  —————————— 几个罕见级别对应的句法元素 Start  —————————— */
    /*
     if( profile_idc = = 100 | | profile_idc = = 110 | |
     profile_idc = = 122 | | profile_idc = = 244 | | profile_idc = = 44 | | profile_idc = = 83 | | profile_idc = = 86 | | profile_idc = = 118 | | profile_idc = = 128 | | profile_idc = = 138 | | profile_idc = = 139 | | profile_idc = = 134 | | profile_idc = = 135 ) {
     */
        int chroma_format_idc;                                  // ue(v)
//      if( chroma_format_idc = = 3 )
            int separate_colour_plane_flag;                     // u(1)
        int bit_depth_luma_minus8;                              // ue(v)
        int bit_depth_chroma_minus8;                            // ue(v)
        int qpprime_y_zero_transform_bypass_flag;               // u(1)
    
        int seq_scaling_matrix_present_flag;                    // u(1)
//      if( seq_scaling_matrix_present_flag )
//          for( i = 0; i < ( ( chroma_format_idc != 3 ) ? 8 : 12 ); i++ ) {
                int seq_scaling_list_present_flag[12]; // 最大值12数组   // u(1)
//              if( seq_scaling_list_present_flag[ i ] )
//                  if( i < 6 )
                        int ScalingList4x4[6][16]; // 二维数组遍历
                        int UseDefaultScalingMatrix4x4Flag[6];
//                  else
                        int ScalingList8x8[6][64];
                        int UseDefaultScalingMatrix8x8Flag[6];
    /*  —————————— 几个罕见级别对应的句法元素 End  —————————— */
    
    /*  —————————— 用来计算POC的句法元素 Start  —————————— */
    int log2_max_frame_num_minus4;                              // ue(v)
    int pic_order_cnt_type;                                     // ue(v)
    
//  if( pic_order_cnt_type = = 0 )
        int log2_max_pic_order_cnt_lsb_minus4;                  // ue(v)
//  else if( pic_order_cnt_type = = 1 ) {
        int delta_pic_order_always_zero_flag;                   // u(1)
        int offset_for_non_ref_pic;                             // se(v)
        int offset_for_top_to_bottom_field;                     // se(v)
        int num_ref_frames_in_pic_order_cnt_cycle;              // ue(v)
//      for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
            int offset_for_ref_frame[256]; // 最大值256数组       // se(v)
    /*  —————————— 用来计算POC的句法元素 End  —————————— */
    
    int max_num_ref_frames;                                     // ue(v)
    int gaps_in_frame_num_value_allowed_flag;                   // u(1)
    
    /*  —————————— 图像宽高相关 Start  —————————— */
    int pic_width_in_mbs_minus1;                                // ue(v)
    int pic_height_in_map_units_minus1;                         // ue(v)
    int frame_mbs_only_flag;                                    // u(1)
//  if( !frame_mbs_only_flag )
        int mb_adaptive_frame_field_flag;                       // u(1)
    /*  —————————— 图像宽高相关 End  —————————— */
    
    int direct_8x8_inference_flag;                              // u(1)
    
    /*  —————————— 解码后图像剪裁的几个句法元素 Start  —————————— */
    int frame_cropping_flag;                                    // u(1)
//  if( frame_cropping_flag ) {
        int frame_crop_left_offset;                             // ue(v)
        int frame_crop_right_offset;                            // ue(v)
        int frame_crop_top_offset;                              // ue(v)
        int frame_crop_bottom_offset;                           // ue(v)
    /*  —————————— 解码后图像剪裁的几个句法元素 End  —————————— */
    
    int vui_parameters_present_flag;                            // u(1)
//  if( vui_parameters_present_flag )
        vui_parameters_t vui_parameters; // Annex E E.1.1
} sps_t;


/**
 Picture Parameter Set
 @see 7.3.2.2 Picture parameter set RBSP syntax
 */
typedef struct
{
    int pic_parameter_set_id;                            // ue(v)
    int seq_parameter_set_id;                            // ue(v)
    int entropy_coding_mode_flag;                        // u(1)
    int bottom_field_pic_order_in_frame_present_flag;    // u(1)
    
    /*  —————————— FMO相关 Start  —————————— */
    int num_slice_groups_minus1;                         // ue(v)
//  if( num_slice_groups_minus1 > 0 ) {
        int slice_group_map_type;                        // ue(v)
//      if( slice_group_map_type = = 0 )
//          for( iGroup = 0; iGroup <= num_slice_groups_minus1; iGroup++ )
                // num_slice_groups_minus1取值范围[0, 7]，见附录A
                int run_length_minus1[8];                // ue(v)
//      else if( slice_group_map_type = = 2 )
//          for( iGroup = 0; iGroup < num_slice_groups_minus1; iGroup++ ) {
                int top_left[8];                         // ue(v)
                int bottom_right[8];                     // ue(v)
//      else if( slice_group_map_type = = 3 | | slice_group_map_type = = 4 | | slice_group_map_type = = 5 ) {
            int slice_group_change_direction_flag;       // u(1)
            int slice_group_change_rate_minus1;          // ue(v)
//      } else if( slice_group_map_type = = 6 ) {
            int pic_size_in_map_units_minus1;            // ue(v)
//          for( i = 0; i <= pic_size_in_map_units_minus1; i++ )
                int *slice_group_id;                     // u(v)
    /*  —————————— FMO相关 End  —————————— */
    
    int num_ref_idx_l0_default_active_minus1;            // ue(v)
    int num_ref_idx_l1_default_active_minus1;            // ue(v)
    int weighted_pred_flag;                              // u(1)
    int weighted_bipred_idc;                             // u(2)
    
    int pic_init_qp_minus26;                             // se(v)
    int pic_init_qs_minus26;                             // se(v)
    int chroma_qp_index_offset;                          // se(v)
    
    int deblocking_filter_control_present_flag;          // u(1)
    int constrained_intra_pred_flag;                     // u(1)
    int redundant_pic_cnt_present_flag;                  // u(1)
    
//  if( more_rbsp_data( ) ) {
        int transform_8x8_mode_flag;                     // u(1)
        int pic_scaling_matrix_present_flag;             // u(1)
//      if( pic_scaling_matrix_present_flag )
            /*
            for( i = 0; i < 6 +
             ( ( chroma_format_idc != 3 ) ? 2 : 6 ) * transform_8x8_mode_flag; i++ ) {
            */
                int pic_scaling_list_present_flag[12];    // u(1)
//              if( pic_scaling_list_present_flag[ i ] )
//                  if( i < 6 )
                        int ScalingList4x4[6][16]; // 二维数组遍历
                        int UseDefaultScalingMatrix4x4Flag[6];
//                  else
                        int ScalingList8x8[6][64];
                        int UseDefaultScalingMatrix8x8Flag[6];
        int second_chroma_qp_index_offset;                  // se(v)
} pps_t;

// 处理SPS
void processSPS(bs_t *b);
// 处理PPS
void processPPS(bs_t *b);

sps_t *allocSPS(void); // 初始化sps结构体
void freeSPS(sps_t *sps); // 释放sps

pps_t *allocPPS(void); // 初始化pps结构体
void freePPS(pps_t *pps); // 释放pps

#endif /* parset_h */
