//
//  slice.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/18.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "slice.h"
#include <stdlib.h>

void processSlice(bs_t *b)
{
    // 0.先解析slice_header
    processSliceHeader(b);
}

// 初始化slice结构体
slice_t *allocSlice(void)
{
    slice_t *slice = calloc(1, sizeof(slice_t));
    if (slice == NULL) {
        fprintf(stderr, "%s\n", "Alloc PPS Error");
        exit(-1);
    }
    return slice;
}

// 释放slice
void freeSlice(slice_t *slice)
{
    free(slice);
}
