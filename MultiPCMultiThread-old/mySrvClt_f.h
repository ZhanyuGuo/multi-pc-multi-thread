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

// ????????????
#define PORT        8888            // ????????????8888
#define BUF_LEN     1024            // ?????????????????????1kB

#ifdef  CLIENT
#define SERVER_IP   "127.0.0.1"     // ?????????IP??????
#endif

// ????????????
#define MAX_THREADS     64          // ????????????64
#define SUBDATANUM      200000     // ??????????????????2000000
#define SRV_SUBDATANUM  100000     // ???PC??????
#define CLT_SUBDATANUM  100000     // ???PC??????
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // ????????????????????????x???????????????

#define S_SUBDATANUM        200000         // ???????????????????????????
#define S_SRV_SUBDATANUM    100000         // ???PC??????????????????
#define S_CLT_SUBDATANUM    100000         // ???PC??????????????????
#define S_DATANUM           (S_SUBDATANUM*MAX_THREADS)      // ????????????(???)????????????x???????????????
#define S_CLT_DATANUM       (S_CLT_SUBDATANUM*MAX_THREADS)  // CLT?????????

#ifdef SERVER
#define S_SRV_DATANUM       (S_SRV_SUBDATANUM*MAX_THREADS)  // SRV?????????
#endif

#define S_ONCE              250                         // ????????????100???float
#define S_TIMES             (S_CLT_DATANUM/S_ONCE)      // ????????????100????????????
#define S_LEFT              (S_CLT_DATANUM%S_ONCE)      // ???????????????????????????

#define DATA_MAX            2147483647                  // ?????????????????????????????????

// ??????????????????
char buf[BUF_LEN];

// ????????????
float rawFloatData[DATANUM];      // ????????????
float floatResults[MAX_THREADS];  // ???????????????
float finalSum;                    // ??????????????????
float finalMax;                    // ????????????????????????

float S_rawFloatData[S_DATANUM];              // ??????????????????
float S_sortFloatData[S_DATANUM];             // ??????????????????
float S_CLT_sortFloatData[S_CLT_DATANUM];     // CLT??????????????????
float S_threadResult[MAX_THREADS][S_SRV_SUBDATANUM];   // ??????????????????????????????
float S_threadResult0[MAX_THREADS][S_SUBDATANUM];      // ??????????????????????????????

#ifdef SERVER
float S_SRV_sortFloatData[S_SRV_DATANUM];     // SRV??????????????????
#endif

// ?????????
bool thread_begin;          // ??????????????????

// ?????????????????????
float NewSum(const float data[], const int len)
{
    float rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

// SSE????????????
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
        xfsload = _mm_log_ps(_mm_sqrt_ps(_mm_load_ps(p)));     // ???????????????
        xfssum = _mm_add_ps(xfssum, xfsload);
        p += nBlockWidth;
    }

    q = (const float*)&xfssum;
    for (int i = 0; i < nBlockWidth ; i++)
    {
        sum += q[i];
    }

    // ???????????????
    for (int i = 0; i < cntRem; i++)
    {
        sum += p[i];
    }

    return sum;
}

// ???????????????????????????
float NewMax(const float data[], const int len)
{
    float max = log10(sqrt(data[0]/4.0));

    for (int i = 1; i < len; i++) if (log10(sqrt(data[i]/4.0)) > max) max = log10(sqrt(data[i]/4.0));

    return max;
}

// ??????????????????????????????????????????
float NewMax_2(const float data[], const int len)
{
    float max = data[0];

    for (int i = 1; i < len; i++) if (data[i] > max) max = data[i];

    return max;
}

// ??????????????????
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

// ???????????????????????????
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

// ????????????
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
