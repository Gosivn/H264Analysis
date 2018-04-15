//
//  h264_nal.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/4/15.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "h264_nal.h"

// 全局变量：nalu缓冲区
uint8_t nalu_buf[1024*1024];

/**
 找到h264码流中的nalu
 [h264协议文档位置]：Annex B

 @param buff h264码流
 @param buff_size 码流大小
 @param curr_nal_start 当前找到的nalu的起始位置
 @param curr_find_index 当前读取的指针位置
 @return nalu的大小
 */
int find_nal_unit(uint8_t *buff, int buff_size, int *curr_nal_start, int *curr_find_index) {
    
    int *i = curr_find_index;
    
    //( next_bits( 24 ) != 0x000001 && next_bits( 32 ) != 0x00000001 )
    // 寻找起始码，只要有一位不满足，则继续向下寻找
    while (
           (buff[*i] != 0x00 || buff[*i+1] != 0x00 || buff[*i+2] != 0x01) &&
           (buff[*i] != 0x00 || buff[*i+1] != 0x00 || buff[*i+2] != 0x00 || buff[*i+3] != 0x01)
           ) {
        
        *i = *i + 1;
        if (*i+3 > buff_size) {return 0;} // 没有找到，退出函数
    }
    
    // 找到起始码，判断如果不是0x000001，则是0x00000001，则将读取索引加1
    // if( next_bits( 24 ) != 0x000001 )
    if (buff[*i] != 0x00 || buff[*i+1] != 0x00 || buff[*i+2] != 0x01) {
        
        *i = *i + 1; // 读取索引加1
    }
    
    *i += 3; // 读取索引加3
    *curr_nal_start = *i;
    
    // 到达nalu部分
    int j = 0;
    // 寻找结尾
    //( next_bits( 24 ) != 0x000000 && next_bits( 24 ) != 0x000001 )
    while (
           (buff[*i] != 0x00 || buff[*i+1] != 0x00 || buff[*i+2] != 0x00) &&
           (buff[*i] != 0x00 || buff[*i+1] != 0x00 || buff[*i+2] != 0x01)
           ) {
        
        nalu_buf[j] = buff[*i]; // 将读取到的nalu存放在全局变量nalu当中
        j++;
        *i = *i +1;
        if (*i+3 >= buff_size) { // 寻找到文件结尾
       
            nalu_buf[j] = buff[*i];
            nalu_buf[j+1] = buff[*i+1];
            nalu_buf[j+2] = buff[*i+2];
            nalu_buf[j+3] = buff[*i+3];
            return buff_size - *curr_nal_start;
        }
    }
    
    return *curr_find_index - *curr_nal_start;
}

