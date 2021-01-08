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

inline __m128 _mm_log_ps(__m128 x)
{
	static const int _ps_min_norm_pos[4] = { 0x00800000, 0x00800000, 0x00800000, 0x00800000 };
	static const int _ps_inv_mant_mask[4] = { ~0x7f800000, ~0x7f800000, ~0x7f800000, ~0x7f800000 };
	static const int _pi32_0x7f[4] = { 0x7f, 0x7f, 0x7f, 0x7f };
	static const float _ps_1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const float _ps_0p5[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
	static const float _ps_sqrthf[4] = { 0.707106781186547524f, 0.707106781186547524f, 0.707106781186547524f, 0.707106781186547524f };
	static const float _ps_log_p0[4] = { 7.0376836292E-2f, 7.0376836292E-2f, 7.0376836292E-2f, 7.0376836292E-2f };
	static const float _ps_log_p1[4] = { -1.1514610310E-1f, -1.1514610310E-1f, -1.1514610310E-1f, -1.1514610310E-1f };
	static const float _ps_log_p2[4] = { 1.1676998740E-1f, 1.1676998740E-1f, 1.1676998740E-1f, 1.1676998740E-1f };
	static const float _ps_log_p3[4] = { -1.2420140846E-1f, -1.2420140846E-1f, -1.2420140846E-1f, -1.2420140846E-1f };
	static const float _ps_log_p4[4] = { 1.4249322787E-1f, 1.4249322787E-1f, 1.4249322787E-1f, 1.4249322787E-1f };
	static const float _ps_log_p5[4] = { -1.6668057665E-1f, -1.6668057665E-1f, -1.6668057665E-1f, -1.6668057665E-1f };
	static const float _ps_log_p6[4] = { 2.0000714765E-1f, 2.0000714765E-1f, 2.0000714765E-1f, 2.0000714765E-1f };
	static const float _ps_log_p7[4] = { -2.4999993993E-1f, -2.4999993993E-1f, -2.4999993993E-1f, -2.4999993993E-1f };
	static const float _ps_log_p8[4] = { 3.3333331174E-1f, 3.3333331174E-1f, 3.3333331174E-1f, 3.3333331174E-1f };
	static const float _ps_log_q1[4] = { -2.12194440e-4f, -2.12194440e-4f, -2.12194440e-4f, -2.12194440e-4f };
	static const float _ps_log_q2[4] = { 0.693359375f, 0.693359375f, 0.693359375f, 0.693359375f };

	__m128 one = *(__m128*)_ps_1;
	__m128 invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());
	/* cut off denormalized stuff */
	x = _mm_max_ps(x, *(__m128*)_ps_min_norm_pos);
	__m128i emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);

	/* keep only the fractional part */
	x = _mm_and_ps(x, *(__m128*)_ps_inv_mant_mask);
	x = _mm_or_ps(x, _mm_set1_ps(0.5f));

	emm0 = _mm_sub_epi32(emm0, *(__m128i*)_pi32_0x7f);
	__m128 e = _mm_cvtepi32_ps(emm0);
	e = _mm_add_ps(e, one);

	__m128 mask = _mm_cmplt_ps(x, *(__m128*)_ps_sqrthf);
	__m128 tmp = _mm_and_ps(x, mask);
	x = _mm_sub_ps(x, one);
	e = _mm_sub_ps(e, _mm_and_ps(one, mask));
	x = _mm_add_ps(x, tmp);

	__m128 z = _mm_mul_ps(x, x);
	__m128 y = *(__m128*)_ps_log_p0;
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p1);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p2);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p3);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p4);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p5);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p6);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p7);
	y = _mm_mul_ps(y, x);
	y = _mm_add_ps(y, *(__m128*)_ps_log_p8);
	y = _mm_mul_ps(y, x);

	y = _mm_mul_ps(y, z);
	tmp = _mm_mul_ps(e, *(__m128*)_ps_log_q1);
	y = _mm_add_ps(y, tmp);
	tmp = _mm_mul_ps(z, *(__m128*)_ps_0p5);
	y = _mm_sub_ps(y, tmp);
	tmp = _mm_mul_ps(e, *(__m128*)_ps_log_q2);
	x = _mm_add_ps(x, y);
	x = _mm_add_ps(x, tmp);
	x = _mm_or_ps(x, invalid_mask); // negative arg will be NAN

	return x;
}

// 网络相关
#define PORT        8888            // 端口号：8888
#define BUF_LEN     1024            // 文字缓存长度：1kB

#ifdef  CLIENT
#define SERVER_IP   "127.0.0.1"     // 服务端IP地址
#endif

// 线程相关
#define MAX_THREADS     64          // 线程数：64
#define SUBDATANUM      200000     // 子块数据量：2000000
#define SRV_SUBDATANUM  100000     // 单PC数据
#define CLT_SUBDATANUM  100000     // 单PC数据
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量

#define S_SUBDATANUM        200000         // 减少数据量进行排序
#define S_SRV_SUBDATANUM    100000         // 单PC小数据量测试
#define S_CLT_SUBDATANUM    100000         // 单PC小数据量测试
#define S_DATANUM           (S_SUBDATANUM*MAX_THREADS)      // 总数据量(小)：线程数x子块数据量
#define S_CLT_DATANUM       (S_CLT_SUBDATANUM*MAX_THREADS)  // CLT数据量

#ifdef SERVER
#define S_SRV_DATANUM       (S_SRV_SUBDATANUM*MAX_THREADS)  // SRV数据量
#endif

#define S_ONCE              250                         // 一次发送100个float
#define S_TIMES             (S_CLT_DATANUM/S_ONCE)      // 总共发送100个的次数
#define S_LEFT              (S_CLT_DATANUM%S_ONCE)      // 发送剩余不足的数据

#define DATA_MAX            2147483647                  // 随机数可能产生的最大值

// 网络收发缓存
char buf[BUF_LEN];

// 计算变量
float rawFloatData[DATANUM];      // 原始数据
float floatResults[MAX_THREADS];  // 各线程结果
float finalSum;                    // 求和最终结果
float finalMax;                    // 求最大值最终结果

float S_rawFloatData[S_DATANUM];              // 排序原始数据
float S_sortFloatData[S_DATANUM];             // 排序最终结果
float S_CLT_sortFloatData[S_CLT_DATANUM];     // CLT排序最终结果
float S_threadResult[MAX_THREADS][S_SRV_SUBDATANUM];   // 单机多线程各线程结果
float S_threadResult0[MAX_THREADS][S_SUBDATANUM];      // 多机多线程各线程结果

#ifdef SERVER
float S_SRV_sortFloatData[S_SRV_DATANUM];     // SRV排序最终结果
#endif

// 标志位
bool thread_begin;          // 线程发令标志

// 不加速版本求和
float NewSum(const float data[], const int len)
{
    float rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

// SSE加速求和
float NewSumSSE(const float* pbuf, const int len)
{
    float sum = 0;

    int nBlockWidth = 4;
    int cntBlock = len/nBlockWidth;
    int cntRem = len%nBlockWidth;

    const float* p = pbuf;
    const float* q;

    __m128 xfsload;
    __m128 xfssum = _mm_setzero_ps();

    for (int i = 0; i < cntBlock; i++)
    {
        xfsload = _mm_log_ps(_mm_sqrt_ps(_mm_load_ps(p)));     // 抓两个出来
        xfssum = _mm_add_ps(xfssum, xfsload);
        p += nBlockWidth;
    }

    q = (const float*)&xfssum;
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
float NewMax(const float data[], const int len)
{
    float max = log10(sqrt(data[0]/4.0));

    for (int i = 1; i < len; i++) if (log10(sqrt(data[i]/4.0)) > max) max = log10(sqrt(data[i]/4.0));

    return max;
}

// 单纯求最大值（用于归并结果）
float NewMax_2(const float data[], const int len)
{
    float max = data[0];

    for (int i = 1; i < len; i++) if (data[i] > max) max = data[i];

    return max;
}

// 快速排序算法
void qsort(float s[], int l, int r)
{
    if (l < r)
    {
        int i = l, j = r;
        float x = s[i];
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
float NewSort(const float data[], const int len, float result[])
{
    int l = 0, r = len - 1;
    for (int i = 0; i < len; i++)
    {
        result[i] = data[i];
    }
    qsort(result, l, r);
}

float NewSort_0(const float data[], const int len, float result[])
{
    float tmp;

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
int check(const float data[], const int len)
{
    float sign;
    float new_sign;

    sign = data[1] - data[0];
    for (int i = 1; i < len - 1; i++)
    {
        new_sign = data[i + 1] - data[i];
        if (new_sign*sign < 0)
        {
            printf("i = %d, %f, %f\r\n", i, data[i], data[i + 1]);
            return 0;
        }
    }

    return 1;
}

#endif // __MY_SRVCLT_H
