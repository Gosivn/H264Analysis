//
//  stream.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/16.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "stream.h"

uint8_t *file_buff;

// 读取h264文件，读取失败返回-1，否则返回文件大小
int readAnnexbBitStreamFile(char *fp)
{
    FILE *fp_h264 = fopen(fp, "rb");
    if (fp_h264 == NULL) {
        printf("打开h264文件失败\n");
        return -1;
    }
    
    file_buff = (uint8_t *)malloc(MAX_BUFFER_SIZE);
    int file_size = (int)fread(file_buff, sizeof(uint8_t), MAX_BUFFER_SIZE, fp_h264);
    fclose(fp_h264);
    return file_size;
}

void freeFilebuffer(void)
{
    free(file_buff);
}
