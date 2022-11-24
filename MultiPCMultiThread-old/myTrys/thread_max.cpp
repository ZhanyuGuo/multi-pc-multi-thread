#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

#define MAX_THREADS     64      //64
#define SUBDATANUM      2000000  //2000000
#define DATANUM         (SUBDATANUM * MAX_THREADS)

double rawDoubleData[DATANUM];
double doubleResults[MAX_THREADS];
bool thread_flag = false;


double NewMax(const double data[],const int len)
{
    double max = data[0];

    for (size_t i = 0; i < len; i++) if (data[i] > max) max = data[i];

    return max;
}

void* threadFunc(void *arg)
{
    int who = *(int *)arg;

    double data[SUBDATANUM];

    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    while (!thread_flag) {}

    doubleResults[who] = NewMax(data, SUBDATANUM);

    return NULL;
}

int main(int argc, char const *argv[])
{
    int id[MAX_THREADS];
    for (size_t i = 0; i < MAX_THREADS; i++) id[i] = i;
    // set stack size to 16M

    struct timeval startv, endv;
    struct timezone startz, endz;

    // long t_nsec = 0;
    long t_usec = 0;

    for (int i = 0; i < DATANUM; i++)
    {
        rawDoubleData[i] = log(sqrt(double(i+1)));
    }

    double finalResult = 0.0f;

    gettimeofday(&startv, &startz);
    finalResult = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);

    printf("test_1: answer = %f, cost = %ld us\r\n", finalResult, t_usec);

    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);

    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&tid[i], &attr, threadFunc, &id[i]);
    }
    
    gettimeofday(&startv, &startz);
    thread_flag = true;

    // wait for ending
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    finalResult = 0.0f;
    finalResult=NewMax(doubleResults,sizeof(doubleResults)/sizeof(double));

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("test_2: answer = %f, cost = %ld us\r\n", finalResult, t_usec);

    return 0;
}