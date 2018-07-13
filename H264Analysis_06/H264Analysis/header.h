//
//  header.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/18.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#ifndef header_h
#define header_h

#include <stdio.h>

/**
 dec_ref_pic_marking()
 [h264协议文档位置]：7.3.3.3 Decoded reference picture marking syntax
 */
typedef struct
{
//  if( IdrPicFlag ) {
        int no_output_of_prior_pics_flag;                   // u(1)
        int long_term_reference_flag;                       // u(1)
//  } else {
        int adaptive_ref_pic_marking_mode_flag;             // u(1)
//      if( adaptive_ref_pic_marking_mode_flag )
//          do{
                int memory_management_control_operation[64];    // ue(v)
//              if( memory_management_control_operation = = 1 | | memory_management_control_operation = = 3 )
                    int difference_of_pic_nums_minus1[64];      // ue(v)
//              if(memory_management_control_operation = = 2 )
                    int long_term_pic_num[64];                  // ue(v)
//              if( memory_management_control_operation = = 3 | | memory_management_control_operation = = 6 )
                    int long_term_frame_idx[64];                // ue(v)
//              if( memory_management_control_operation = = 4 )
                    int max_long_term_frame_idx_plus1[64];      // ue(v)
//          } while( memory_management_control_operation != 0 )
    
} dec_ref_pic_marking_t;

/**
 pred_weight_table()
 [h264协议文档位置]：7.3.3.2 Prediction weight table syntax
 */
typedef struct
{
    int luma_log2_weight_denom;                         // ue(v)
//  if( ChromaArrayType != 0 )
        int chroma_log2_weight_denom;                   // ue(v)
    
//  for( i = 0; i <= num_ref_idx_l0_active_minus1; i++ ) {
        int luma_weight_l0_flag;                        // u(1)
//      if( luma_weight_l0_flag ) {
            int luma_weight_l0[32];                     // se(v)
            int luma_offset_l0[32];                     // se(v)
    
//      if( ChromaArrayType != 0 ) {
            int chroma_weight_l0_flag;                  // u(1)
//          if( chroma_weight_l0_flag )
//              for( j =0; j < 2; j++ ) {
                    int chroma_weight_l0[32][2];        // se(v)
                    int chroma_offset_l0[32][2];        // se(v)
    
//  if( slice_type % 5 = = 1 )
//      for( i = 0; i <= num_ref_idx_l1_active_minus1; i++ ) {
            int luma_weight_l1_flag;                    // u(1)
//          if( luma_weight_l1_flag ) {
                int luma_weight_l1[32];                 // se(v)
                int luma_offset_l1[32];                 // se(v)
    
//          if( ChromaArrayType != 0 ) {
                int chroma_weight_l1_flag;              // u(1)
//              if( chroma_weight_l1_flag )
//                  for( j = 0; j < 2; j++ ) {
                        int chroma_weight_l1[32][2];    // se(v)
                        int chroma_offset_l1[32][2];    // se(v)
    
} pred_weight_table_t;

/**
 2005.03版h264：ref_pic_list_reordering()
 2017.04版h264：ref_pic_list_modification()
 下面以最新版也即2017.04版进行定义
 [h264协议文档位置]：7.3.3.1 Reference picture list modification syntax
 */
typedef struct
{
//  if( slice_type % 5 != 2 && slice_type % 5 != 4 ) {
        int ref_pic_list_modification_flag_l0;        // u(1)
//      if( ref_pic_list_modification_flag_l0 )
//      do {
            int *modification_of_pic_nums_idc_lo;         // ue(v)
//          if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 )
                int *abs_diff_pic_num_minus1_lo;          // ue(v)
//          else if( modification_of_pic_nums_idc = = 2 )
                int *long_term_pic_num_lo;                // ue(v)
//      } while( modification_of_pic_nums_idc != 3 )
    
//  if( slice_type % 5 = = 1 ) {
        int ref_pic_list_modification_flag_l1;         // u(1)
//      if( ref_pic_list_modification_flag_l1 )
//      do {
            int *modification_of_pic_nums_idc_l1;         // ue(v)
//          if( modification_of_pic_nums_idc = = 0 | | modification_of_pic_nums_idc = = 1 )
                int *abs_diff_pic_num_minus1_l1;          // ue(v)
//          else if( modification_of_pic_nums_idc = = 2 )
                int *long_term_pic_num_l1;                // ue(v)
//      } while( modification_of_pic_nums_idc != 3 )
    
} rplm_t; // ref_pic_list_modification()

/**
 slice_header( )
 [h264协议文档位置]：7.3.3 Slice header syntax
 */
typedef struct
{
    int first_mb_in_slice;                                // ue(v)
    int slice_type;                                       // ue(v)
    int pic_parameter_set_id;                             // ue(v)
//  if( separate_colour_plane_flag = = 1 )
        int colour_plane_id;                              // u(2)
    int frame_num;                                        // u(v)
//  if( !frame_mbs_only_flag ) {
        int field_pic_flag;                               // u(1)
//      if( field_pic_flag )
            int bottom_field_flag;                        // u(1)
//  if( IdrPicFlag )
        int idr_pic_id;                                   // ue(v)
    
//  if( pic_order_cnt_type = = 0 ) {
        int pic_order_cnt_lsb;                            // u(v)
//      if( bottom_field_pic_order_in_frame_present_flag && !field_pic_flag )
            int delta_pic_order_cnt_bottom;               // se(v)
//  if( pic_order_cnt_type = = 1 && !delta_pic_order_always_zero_flag ) {
        int delta_pic_order_cnt[2];                       // se(v)
    
//  if( redundant_pic_cnt_present_flag )
        int redundant_pic_cnt;                            // ue(v)
    
//  if( slice_type = = B )
        int direct_spatial_mv_pred_flag;                  // u(1)
//  if(slice_type == P || slice_type == SP || slice_type == B){
        int num_ref_idx_active_override_flag;             // u(1)
//      if( num_ref_idx_active_override_flag ) {
            int num_ref_idx_l0_active_minus1;             // ue(v)
//          if( slice_type = = B )
                int num_ref_idx_l1_active_minus1;         // ue(v)
    
//  7.3.3.1 Reference picture list modification syntax
    rplm_t ref_pic_list_modification;
    
//  if( ( weighted_pred_flag && ( slice_type = = P | | slice_type = = SP ) ) | | ( weighted_bipred_idc = = 1 && slice_type = = B ) )
        pred_weight_table_t pred_weight_table;
    
//  if( nal_ref_idc != 0 )
        dec_ref_pic_marking_t dec_ref_pic_marking;
    
//  if( entropy_coding_mode_flag && slice_type != I && slice_type != SI )
        int cabac_init_idc;                               // ue(v)
    int slice_qp_delta;                                   // se(v)
//  if(slice_type == SP || slice_type == SI){
//      if( slice_type = = SP )
            int sp_for_switch_flag;                       // u(1)
        int slice_qs_delta;                               // se(v)
    
//  if( deblocking_filter_control_present_flag ) {
        int disable_deblocking_filter_idc;                // ue(v)
//      if( disable_deblocking_filter_idc != 1 ) {
            int slice_alpha_c0_offset_div2;               // se(v)
            int slice_beta_offset_div2;                   // se(v)
    
//  if( num_slice_groups_minus1 > 0 &&
//      slice_group_map_type >= 3 && slice_group_map_type <= 5)
        int slice_group_change_cycle;                     // u(v)
    
} slice_header_t;

//void processSlice(slice_t *slice);

#endif /* header_h */
