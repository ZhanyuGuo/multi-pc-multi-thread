#include <stdio.h>
#include <stdlib.h>
#include "emmintrin.h"//sse2
#include "tmmintrin.h"//sse3
#include "immintrin.h"//好像包括了所有
#include <stdint.h>
#include<math.h>
#include<time.h>
#include <unistd.h>
#include <sys/time.h>
#include<iostream>

using namespace std;

#define MAX_THREADS     64      //64
#define SUBDATANUM      1000000  //2000000
#define DATANUM         (SUBDATANUM * MAX_THREADS)
double rawDoubleData[DATANUM];
float rawfloatdata[DATANUM];

// double NewSum(const double data[], const int len)
// {
//     double rlt = 0.0f;

//     for (int i = 0; i < len; i++) rlt += sin(sqrt(data[i]));
    
//     return rlt;
// }

double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += sqrt(sqrt(data[i]));
    
    return rlt;
}

float NewSumf(const float data[], const int len)
{
    float rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += sqrtf(sqrtf(data[i]));
    
    return rlt;
}

// double ssesum(const double* pbuf,int len)
// {
//     double sum=0.0f;
//     size_t blockwidth=2;
//     size_t cntblock=len/blockwidth;
//     const double* p=pbuf;
//     const double*q;
//     __m128d xfsload;
//     __m128d xfssum=_mm_setzero_pd();
//     for (size_t i = 0; i <cntblock; i++)
//     {
//         xfsload=_mm_sqrt_pd(_mm_sqrt_pd(_mm_load_pd(p)));//抓两个出来
//         xfssum=_mm_add_pd(xfssum,xfsload);
//         p+=2;
//     }
//     q=(const double*)&xfssum;
//     for (size_t i = 0; i <blockwidth ; i++)
//     {
//         sum+=q[i];
//     }
//     return sum;
// }

float ssesumf(const float* pbuf,int len)
{
    float sum=0;
    size_t blockwidth=4;
    size_t cntblock=len/blockwidth;
    const float* p=pbuf;
    const float*q;
    __m128 xfsload;
    __m128 xfssum=_mm_setzero_ps();
    for (size_t i = 0; i <cntblock; i++)
    {
        xfsload=_mm_sqrt_ps(_mm_sqrt_ps(_mm_load_ps(p)));//抓两个出来
        xfssum=_mm_add_ps(xfssum,xfsload);
        p+=4;
    }
    q=(const float*)&xfssum;
    for (size_t i = 0; i <blockwidth ; i++)
    {
        sum+=q[i];
    }
    return sum;
}

// double ssesum_256(const double* pbuf,int len)
// {
//     double sum=0.0f;
//     size_t blockwidth=2;
//     size_t cntblock=len/blockwidth;
//     const double* p=pbuf;
//     const double*q;
//     __m256d xfsload;
//     __m256d xfssum=_mm256_setzero_pd();
//     for (size_t i = 0; i <cntblock; i++)
//     {
//         xfsload=_mm256_sqrt_pd(_mm256_load_pd(p));//抓两个出来
//         xfssum=_mm256_add_pd(xfssum,xfsload);
//         p+=blockwidth;
    
//     }
//     q=(const double*)&xfssum;
//     for (size_t i = 0; i <blockwidth ; i++)
//     {
//         sum+=q[i];
//     }
//     return sum;
// }

int main()
{
    struct timeval startv, endv;
    struct timezone startz, endz;
    long t_usec = 0;

    for (int i = 0; i < DATANUM; i++)
    {
        //rawDoubleData[i] = double(i%100);
        rawfloatdata[i] = float(i);
    }
    float finalResult = 0.0f;
    gettimeofday(&startv, &startz);
    finalResult = NewSumf(rawfloatdata, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("test_1: answer = %f, cost = %ld us\r\n", finalResult, t_usec);


    finalResult = 0.0f;
    gettimeofday(&startv, &startz);
    finalResult = ssesumf(rawfloatdata, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("test_2: answer = %f, cost = %ld us\r\n", finalResult, t_usec);



    return 0;
}