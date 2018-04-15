//
//  main.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/4/15.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "h264_nal.h"

#define MAX_BUFFER_SIZE (50*1024*1024)

int main(int argc, const char * argv[]) {
    
    FILE *fp_h264 = fopen("test.h264", "rb");
    
    if (fp_h264 == NULL) {
        printf("打开h264文件失败");
        return -1;
    }
    
    // 0.使用指针指向的堆空间作为缓冲区
    uint8_t *buff = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    
    // 1.一次读进缓冲区
    int buff_size = (int)fread(buff, sizeof(uint8_t), MAX_BUFFER_SIZE, fp_h264);
    
    printf("totalSize: %d\n", buff_size);
    
    
    extern uint8_t nalu_buf[];

    int nalu_i = 0;
    int nalu_size = 0;
    int curr_nal_start = 0;  // 当前找到的nalu起始位置
    int curr_find_index = 0; // 当前查找的位置索引
    
    // 3.找到h264码流中的各个nalu
    while ((nalu_size = find_nal_unit(buff, buff_size, &curr_nal_start, &curr_find_index)) > 0) {
        printf("nalu: %d, start: %d, index: %d, size: %d\n", nalu_i, curr_nal_start, curr_find_index, curr_find_index - curr_nal_start);
        
//        for (int i = 0; i < nalu_size; i++) {
//            printf("%x", nalu_buf[i]);
//        }
//        printf("\n");
        nalu_i++;
    }
    
    free(buff);
    fclose(fp_h264);
    
    return 0;
}
