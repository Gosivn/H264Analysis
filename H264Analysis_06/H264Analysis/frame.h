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

typedef enum
{
    // YUV
    COLOR_PLANE_Y = 0,  // PLANE_Y
    COLOR_PLANE_U = 1,  // PLANE_Cb
    COLOR_PLANE_V = 2,  // PLANE_Cr
    // RGB
    COLOR_PLANE_G = 0,
    COLOR_PLANE_B = 1,
    COLOR_PLANE_R = 2
} Color_Plane;

#endif /* frame_h */
