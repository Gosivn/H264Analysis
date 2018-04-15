//
//  h264_nal.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/4/15.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#ifndef h264_nal_h
#define h264_nal_h

#include <stdio.h>
#include <stdlib.h>

int find_nal_unit(uint8_t *buff, int buff_size, int *curr_nal_start, int *curr_find_index);

#endif /* h264_nal_h */
