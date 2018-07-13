//
//  nalu.c
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/16.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

#include "nalu.h"
#include <assert.h>
#include "stream.h"
#include "parset.h"
#include "slice.h"

extern slice_t *currentSlice;

/**
 找到h264码流中的nalu
 [h264协议文档位置]：Annex B
 
 @param nalu 保存找到的nalu->buf
 @param buff_size 码流大小
 @param curr_nal_start 当前找到的nalu的起始位置
 @param curr_find_index 当前读取的指针位置
 @return nalu的大小
 */
int find_nal_unit(nalu_t *nalu, int buff_size, int *curr_nal_start, int *curr_find_index) {
    
    int *i = curr_find_index;
    
    //( next_bits( 24 ) != 0x000001 && next_bits( 32 ) != 0x00000001 )
    // 寻找起始码，只要有一位不满足，则继续向下寻找
    while (
           (file_buff[*i] != 0x00 || file_buff[*i+1] != 0x00 || file_buff[*i+2] != 0x01) &&
           (file_buff[*i] != 0x00 || file_buff[*i+1] != 0x00 || file_buff[*i+2] != 0x00 || file_buff[*i+3] != 0x01)
           ) {
        
        *i = *i + 1;
        if (*i+3 > buff_size) {return 0;} // 没有找到，退出函数
    }
    
    // 找到起始码，判断如果不是0x000001，则是0x00000001，则将读取索引加1
    // if( next_bits( 24 ) != 0x000001 )
    if (file_buff[*i] != 0x00 || file_buff[*i+1] != 0x00 || file_buff[*i+2] != 0x01) {
        
        *i = *i + 1; // 读取索引加1
    }
    
    *i += 3; // 读取索引加3
    *curr_nal_start = *i;
    
    // 到达nalu部分
    int j = 0;
    // 寻找结尾
    //( next_bits( 24 ) != 0x000000 && next_bits( 24 ) != 0x000001 )
    while (
           (file_buff[*i] != 0x00 || file_buff[*i+1] != 0x00 || file_buff[*i+2] != 0x00) &&
           (file_buff[*i] != 0x00 || file_buff[*i+1] != 0x00 || file_buff[*i+2] != 0x01)
           ) {
        
        nalu->buf[j] = file_buff[*i]; // 将读取到的nalu存放在全局变量nalu当中
        j++;
        *i = *i +1;
        if (*i+3 >= buff_size) { // 寻找到文件结尾
            
            nalu->buf[j] = file_buff[*i];
            nalu->buf[j+1] = file_buff[*i+1];
            nalu->buf[j+2] = file_buff[*i+2];
            nalu->buf[j+3] = file_buff[*i+3];
            return buff_size - *curr_nal_start;
        }
    }
    
    return *curr_find_index - *curr_nal_start;
}

/**
 读取一个nalu
 @see 7.3.1 NAL unit syntax
 @see 7.4.1 NAL unit semantics
 */
void read_nal_unit(nalu_t *nalu)
{
    int nalu_size = nalu->len;
    
    // 1.去除nalu中的emulation_prevention_three_byte：0x03
    nalu->len = nal_to_rbsp(nalu);
    
    // 2.初始化逐比特读取工具句柄
    bs_t *bs = bs_new(nalu->buf, nalu->len);
    
    // 3. 读取nal header 7.3.1
    nalu->forbidden_zero_bit = bs_read_u(bs, 1, NULL);
    nalu->nal_ref_idc = bs_read_u(bs, 2, NULL);
    nalu->nal_unit_type = bs_read_u(bs, 5, NULL);
    
    printf("\tnal->forbidden_zero_bit: %d\n", nalu->forbidden_zero_bit);
    printf("\tnal->nal_ref_idc: %d\n", nalu->nal_ref_idc);
    printf("\tnal->nal_unit_type: %d\n", nalu->nal_unit_type);

#if TRACE
    fprintf (trace_fp, "\n\nAnnex B NALU len %d, forbidden_bit %d, nal_reference_idc %d, nal_unit_type %d\n\n", nalu_size, nalu->forbidden_zero_bit, nalu->nal_ref_idc, nalu->nal_unit_type);
    fflush (trace_fp);
#endif
    
    switch (nalu->nal_unit_type)
    {
        case H264_NAL_SPS:
            nalu->len = rbsp_to_sodb(nalu);
            processSPS(bs);
            break;
            
        case H264_NAL_PPS:
            nalu->len = rbsp_to_sodb(nalu);
            processPPS(bs);
            break;
            
        case H264_NAL_SLICE:
        case H264_NAL_IDR_SLICE:
            currentSlice->idr_flag = (nalu->nal_unit_type == H264_NAL_IDR_SLICE);
            currentSlice->nal_ref_idc = nalu->nal_ref_idc;
            nalu->len = rbsp_to_sodb(nalu);
            processSlice(bs);
            break;
            
        case H264_NAL_DPA:
            nalu->len = rbsp_to_sodb(nalu);
            break;
            
        case H264_NAL_DPB:
            nalu->len = rbsp_to_sodb(nalu);
            break;
            
        case H264_NAL_DPC:
            nalu->len = rbsp_to_sodb(nalu);
            break;
            
        default:
            break;
    }
    
    bs_free(bs);
}

/**
 去除rbsp中的0x03
 @see 7.3.1 NAL unit syntax
 @see 7.4.1.1 Encapsulation of an SODB within an RBSP
 @return 返回去除0x03后nalu的大小
 */
int nal_to_rbsp(nalu_t *nalu)
{
    
    int nalu_size = nalu->len;
    int j = 0;
    int count = 0;
    
    // 遇到0x000003则把03去掉，包含以cabac_zero_word结尾时，尾部为0x000003的情况
    for (int i = 0; i < nalu_size; i++)
    {
        
        if (count == 2 && nalu->buf[i] == 0x03)
        {
            if (i == nalu_size - 1) // 结尾为0x000003
            {
                break; // 跳出循环
            }
            else
            {
                i++; // 继续下一个
                count = 0;
            }
        }
        
        nalu->buf[j] = nalu->buf[i];
        
        if (nalu->buf[i] == 0x00)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        
        j++;
    }
    
    return j;
}

/**
 计算SODB的长度
 【注】RBSP = SODB + trailing_bits
 */
int rbsp_to_sodb(nalu_t *nalu)
{
    int ctr_bit, bitoffset, last_byte_pos;
    bitoffset = 0;
    last_byte_pos = nalu->len - 1;
    
    // 0.从nalu->buf的最末尾的比特开始寻找
    ctr_bit = (nalu->buf[last_byte_pos] & (0x01 << bitoffset));
    
    // 1.循环找到trailing_bits中的rbsp_stop_one_bit
    while (ctr_bit == 0)
    {
        bitoffset++;
        if(bitoffset == 8)
        {
            // 因nalu->buf中保存的是nalu_header+RBSP，因此找到最后1字节的nalu_header就宣告RBSP查找结束
            if(last_byte_pos == 1)
                printf(" Panic: All zero data sequence in RBSP \n");
            assert(last_byte_pos != 1);
            last_byte_pos -= 1;
            bitoffset = 0;
        }
        ctr_bit= nalu->buf[last_byte_pos-1] & (0x01 << bitoffset);
    }
    // 【注】函数开始已对last_byte_pos做减1处理，此时last_byte_pos表示相对于SODB的位置，然后赋值给nalu->len得到最终SODB的大小
    return last_byte_pos;
}

/**
 在rbsp_trailing_bits()之前是否有更多数据
 [h264协议文档位置]：7.2 Specification of syntax functions, categories, and descriptors
 */
int more_rbsp_data(bs_t *b)
{
    // 0.是否已读到末尾
    if (bs_eof(b)) {return 0;}
    
    // 1.下一比特值是否为0，为0说明还有更多数据
    if (bs_peek_u1(b) == 0) {return 1;}
    
    // 2.到这说明下一比特值为1，这就要看看是否这个1就是rbsp_stop_one_bit，也即1后面是否全为0
    bs_t bs_temp;
    bs_clone(&bs_temp, b);
    
    // 3.跳过刚才读取的这个1，逐比特查看1后面的所有比特，直到遇到另一个1或读到结束为止
    bs_read_u1(&bs_temp);
    while(!bs_eof(&bs_temp))
    {
        if (bs_read_u1(&bs_temp) == 1) { return 1; }
    }
    
    return 0;
}

nalu_t *allocNalu(int buff_size)
{
    // calloc：初始化所分配的内存空间
    nalu_t *nalu = (nalu_t *)calloc( 1, sizeof(nalu_t));
    if (nalu == NULL) {
        fprintf(stderr, "%s\n", "AllocNALU: nalu");
        exit(-1);
    }
    
    nalu->buf = (uint8_t *)calloc(buff_size, sizeof(uint8_t));
    if (nalu->buf == NULL) {
        fprintf(stderr, "%s\n", "AllocNALU: nalu->buf");
        exit(-1);
    }
    return nalu;
}

void freeNalu(nalu_t *nalu)
{
    free(nalu);
}
