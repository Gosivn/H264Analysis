//
//  bs.h
//  H264Analysis
//
//  Created by Jinmmer on 2018/5/11.
//  Copyright © 2018年 Jinmmer. All rights reserved.
//

/**
 逐bit读取工具
 */

#ifndef bs_h
#define bs_h

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "stream.h"

typedef struct
{
    uint8_t* start; // 指向buf头部指针
    uint8_t* p;     // 当前指针位置
    uint8_t* end;   // 指向buf尾部指针
    int bits_left;  // 当前读取字节的剩余(可用/未读)比特个数
} bs_t;


/** 初始化句柄 */
static inline bs_t* bs_init(bs_t* b, uint8_t* buf, size_t size)
{
    b->start = buf;  // 指向buf起始位置
    b->p = buf;      // 初始位置与start保持一致
    b->end = buf + size;    // 指向buf末尾
    b->bits_left = 8;   // 默认剩余8比特可用
    return b;
}

static inline bs_t* bs_new(uint8_t* buf, size_t size)
{
    bs_t* b = (bs_t*)malloc(sizeof(bs_t));
    bs_init(b, buf, size);
    return b;
}

static inline void bs_free(bs_t* b)
{
    free(b);
}

/** 是否已读到末尾（end_of_file） */
static inline int bs_eof(bs_t* bs) { if (bs->p >= bs->end) { return 1; } else { return 0; } }

/**
 克隆一个bs_t句柄

 @param dest 目标句柄
 @param src 源句柄
 @return 目标句柄
 */
static inline bs_t* bs_clone(bs_t* dest, const bs_t* src)
{
    dest->start = src->p;
    dest->p = src->p;
    dest->end = src->end;
    dest->bits_left = src->bits_left;
    return dest;
}

/** 不影响原读取操作的情况下，读取1个比特 */
static inline uint32_t bs_peek_u1(bs_t* b)
{
    uint32_t r = 0;
    
    if (! bs_eof(b))
    {
        r = ((*(b->p)) >> ( b->bits_left - 1 )) & 0x01;
    }
    return r;
}

/** 读取1个比特 */
static inline uint32_t bs_read_u1(bs_t* b)
{
    uint32_t r = 0; // 读取比特返回值
    
    // 1.剩余比特先减1
    b->bits_left--;
    
    if (! bs_eof(b))
    {
        // 2.计算返回值
        r = ((*(b->p)) >> b->bits_left) & 0x01;
    }
    
    // 3.判断是否读到字节末尾，如果是指针位置移向下一字节，比特位初始为8
    if (b->bits_left == 0) { b->p ++; b->bits_left = 8; }
    
    return r;
}

/**
 读取n个比特
 
 @param b 比特流操作句柄
 @param n 读取多少个比特
 @return 返回读取到的值
 */
static inline uint32_t bs_read_u(bs_t* b, int n, char *traceString)
{
    uint32_t r = 0; // 读取比特返回值
    int i;  // 当前读取到的比特位索引
    for (i = 0; i < n; i++)
    {
        // 1.每次读取1比特，并依次从高位到低位放在r中
        r |= ( bs_read_u1(b) << ( n - i - 1 ) );
    }
    
    if (traceString != NULL) {
#if TRACE
        traceInput(traceString, r);
#endif
    }
    return r;
}

/**
 ue(v) 解码
 */
static inline uint32_t bs_read_ue(bs_t* b, char *traceString)
{
    int32_t r = 0; // 解码得到的返回值
    int i = 0;     // leadingZeroBits
    
    // 1.计算leadingZeroBits
    while( (bs_read_u1(b) == 0) && (i < 32) && (!bs_eof(b)) )
    {
        i++;
    }
    // 2.计算read_bits( leadingZeroBits )
    r = bs_read_u(b, i, NULL);
    // 3.计算codeNum，1 << i即为2的i次幂
    r += (1 << i) - 1;
    
    if (traceString != NULL) {
#if TRACE
        traceInput(traceString, r);
#endif
    }
    return r;
}

/**
 se(v) 解码
 */
static inline int32_t bs_read_se(bs_t* b, char *traceString)
{
    // 1.解码出codeNum，记为r
    int32_t r = bs_read_ue(b, NULL);
    // 2.判断r的奇偶性
    if (r & 0x01) // 如果为奇数，说明编码前>0
    {
        r = (r+1)/2;
    }
    else // 如果为偶数，说明编码前<=0
    {
        r = -(r/2);
    }
    
    if (traceString != NULL) {
#if TRACE
        traceInput(traceString, r);
#endif
    }
    return r;
}

/**
 te(v) 解码
 */
static inline uint32_t bs_read_te( bs_t *b, int x, char *traceString )
{
    uint32_t r = 0;
    
    // 1.判断取值上限
    if( x == 1 ) // 如果为1则将读取到的比特值取反
    {
        r = 1 - bs_read_u1( b );
    }
    else if( x > 1 ) // 否则按照ue(v)进行解码
    {
        r = bs_read_ue( b , NULL);
    }
    
    if (traceString != NULL) {
#if TRACE
        traceInput(traceString, r);
#endif
    }
    return r;
}

#endif /* bs_h */
