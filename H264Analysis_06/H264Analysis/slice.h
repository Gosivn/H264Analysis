//
//  slice.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/18.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#ifndef slice_h
#define slice_h

#include <stdio.h>
#include "header.h"
#include "bs.h"

typedef enum {
    Slice_Type_P = 0,
    Slice_Type_B,
    Slice_Type_I,
    Slice_Type_SP,
    Slice_Type_SI
} Slice_Type;

typedef struct
{
    int idr_flag; // 是否为IDR帧
    int nal_ref_idc; // nalu->nal_ref_idc
    slice_header_t slice_header;
} slice_t;

void processSliceHeader(bs_t *b);
void processSlice(bs_t *b);

slice_t *allocSlice(void); // 初始化slice结构体
void freeSlice(slice_t *slice); // 释放slice

#endif /* slice_h */
