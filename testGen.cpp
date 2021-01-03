#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_THREADS     64          // 线程数：64
#define SUBDATANUM      2000000     // 子块数据量：2000000
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量
double rawDoubleData[DATANUM];      // 原始数据
double finalSum;
// 不加速版本求和
double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

int main(int argc, char const *argv[])
{
    for (int i = 0; i < DATANUM; i++)
    {
        rawDoubleData[i] = fabs(double(rand() + rand() + rand() + rand()));
        if (rawDoubleData[i] == 0) rawDoubleData[i]++;
        // printf("%lf\r\n", rawDoubleData[i]);
    }
    finalSum = NewSum(rawDoubleData, DATANUM);
    printf("finalSum = %lf\r\n", finalSum);
    
    return 0;
}
