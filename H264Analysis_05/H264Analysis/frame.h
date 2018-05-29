//
//  frame.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/16.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#ifndef frame_h
#define frame_h

#include <stdio.h>

typedef enum {
    COLOR_SPACE_FORMAT_UNKNOWN = -1,     //!< Unknown color space format
    YUV_4_0_0     =  0,     //!< 没有色度chrome，只采样亮度luma
    YUV_4_2_0     =  1,     //!< 4:2:0
    YUV_4_2_2     =  2,     //!< 4:2:2
    YUV_4_4_4     =  3      //!< 4:4:4
} Color_Space_Format;

#endif /* frame_h */
