#ifndef __MY_SRVCLT_H
#define __MY_SRVCLT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "emmintrin.h"  //SSE2
#include "tmmintrin.h"  //SSE3
#include "immintrin.h"  //ALL

// 网络相关
#define PORT        8888            // 端口号：8888
#define BUF_LEN     1024            // 文字缓存长度：1kB

#ifdef  CLIENT
#define SERVER_IP   "127.0.0.1"     // 服务端IP地址
#endif

// 线程相关
#define MAX_THREADS     64          // 线程数：64
#define SUBDATANUM      2000000     // 子块数据量：2000000
#define SRV_SUBDATANUM  1000000     // 单PC数据
#define CLT_SUBDATANUM  1000000     // 单PC数据
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量

#define S_SUBDATANUM        200         // 减少数据量进行排序
#define S_SRV_SUBDATANUM    100         // 单PC小数据量测试
#define S_CLT_SUBDATANUM    100         // 单PC小数据量测试
#define S_DATANUM           (S_SUBDATANUM*MAX_THREADS)      // 总数据量(小)：线程数x子块数据量
#define S_CLT_DATANUM       (S_CLT_SUBDATANUM*MAX_THREADS)  // CLT数据量

#ifdef SERVER
#define S_SRV_DATANUM       (S_SRV_SUBDATANUM*MAX_THREADS)  // SRV数据量
#endif

#define S_ONCE              100                         // 一次发送100个double
#define S_TIMES             (S_CLT_DATANUM/S_ONCE)      // 总共发送100个的次数
#define S_LEFT              (S_CLT_DATANUM%S_ONCE)      // 发送剩余不足的数据

#define DATA_MAX            2147483647                  // 随机数可能产生的最大值

// 网络收发缓存
char buf[BUF_LEN];

// 计算变量
double rawDoubleData[DATANUM];      // 原始数据
double doubleResults[MAX_THREADS];  // 各线程结果
double finalSum;                    // 求和最终结果
double finalMax;                    // 求最大值最终结果

double S_rawDoubleData[S_DATANUM];              // 排序原始数据
double S_sortDoubleData[S_DATANUM];             // 排序最终结果
double S_CLT_sortDoubleData[S_CLT_DATANUM];     // CLT排序最终结果
double S_threadResult[MAX_THREADS][S_SRV_SUBDATANUM];   // 单机多线程各线程结果
double S_threadResult0[MAX_THREADS][S_SUBDATANUM];      // 多机多线程各线程结果

#ifdef SERVER
double S_SRV_sortDoubleData[S_SRV_DATANUM];     // SRV排序最终结果
#endif

// 标志位
bool thread_begin;          // 线程发令标志

// 不加速版本求和
double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

// SSE加速求和
double NewSumSSE(const double* pbuf, const int len)
{
    double sum = 0;

    int nBlockWidth = 2;
    int cntBlock = len/nBlockWidth;
    int cntRem = len%nBlockWidth;

    const double* p = pbuf;
    const double* q;

    __m128d xfsload;
    __m128d xfssum = _mm_setzero_pd();

    for (int i = 0; i < cntBlock; i++)
    {
        xfsload = _mm_sqrt_pd(_mm_sqrt_pd(_mm_load_pd(p)));     // 抓两个出来
        xfssum = _mm_add_pd(xfssum, xfsload);
        p += nBlockWidth;
    }

    q = (const double*)&xfssum;
    for (int i = 0; i < nBlockWidth ; i++)
    {
        sum += q[i];
    }

    // 提取剩余的
    for (int i = 0; i < cntRem; i++)
    {
        sum += p[i];
    }

    return sum;
}

// 不加速版本求最大值
double NewMax(const double data[], const int len)
{
    double max = log10(sqrt(data[0]/4.0));

    for (int i = 1; i < len; i++) if (log10(sqrt(data[i]/4.0)) > max) max = log10(sqrt(data[i]/4.0));

    return max;
}

// 单纯求最大值（用于归并结果）
double NewMax_2(const double data[], const int len)
{
    double max = data[0];

    for (int i = 1; i < len; i++) if (data[i] > max) max = data[i];

    return max;
}

// 快速排序算法
void qsort(double s[], int l, int r)
{
    if (l < r)
    {
        int i = l, j = r;
        double x = s[i];
        while (i < j)
        {
            while (i < j && s[j] >= x)
            {
                j--;
            }
            if (i < j)
            {
                s[i++] = s[j];
            }
            while (i < j && s[i] < x)
            {
                i++;
            }
            if (i < j)
            {
                s[j--] = s[i];
            }           
        }
        s[i] = x;
        qsort(s, l, i - 1);
        qsort(s, i + 1, r);
    }
}

// 不加速排序（快排）
double NewSort(const double data[], const int len, double result[])
{
    int l = 0, r = len - 1;
    for (int i = 0; i < len; i++)
    {
        result[i] = data[i];
    }
    qsort(result, l, r);
}

double NewSort_0(const double data[], const int len, double result[])
{
    double tmp;

    for (int i = 0; i < len; i++) result[i] = data[i];
    
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
        {
            if (log10(sqrt(result[j])) > log10(sqrt(result[j + 1])))
            {
                tmp = result[j];
                result[j] = result[j + 1];
                result[j+1] = tmp;
            }
        }
    }
}

// 检验结果
int check(const double data[], const int len)
{
    double sign;
    double new_sign;

    sign = data[1] - data[0];
    for (int i = 1; i < len - 1; i++)
    {
        new_sign = data[i + 1] - data[i];
        if (new_sign*sign < 0)
        {
            // printf("i = %d, %f, %f\r\n", i, data[i], data[i + 1]);
            return 0;
        }
    }

    return 1;
}

#endif // __MY_SRVCLT_H
