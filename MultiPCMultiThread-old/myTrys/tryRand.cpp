#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_THREADS     64          // 线程数：64
#define SUBDATANUM      2000000     // 子块数据量：2000000
#define SRV_SUBDATANUM  1000000     // 单PC数据
#define CLT_SUBDATANUM  1000000     // 单PC数据
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量

double rawDoubleData[DATANUM];

int main(int argc, char const *argv[])
{
    // srand((int)time(NULL));
    srand(1);
    // double a = rand();
    // for (size_t i = 0; i < 100; i++)
    // {
    //     printf("%d\r\n", rand()%100000);
    // }
    for (size_t i = 0; i < DATANUM; i++)
    {
        rawDoubleData[i] = double(rand()+rand()+rand()+rand());
        printf("%lf\r\n", rawDoubleData[i]);
    }
    
    printf("Finished...\r\n");
    return 0;
}
