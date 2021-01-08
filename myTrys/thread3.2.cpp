#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <time.h>


#define MAX_THREADS     64          // 64
#define SUBDATANUM      2000000     // 2000000
#define DATANUM         (SUBDATANUM * MAX_THREADS)
// #define USLEEP_T        10000

double rawDoubleData[DATANUM];
double doubleResults[MAX_THREADS];
bool thread_begin = false;


double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += sin(sqrt(data[i]));
    
    return rlt;
}


void* threadFunc(void *arg)
{
    int who = *(int *)arg;
    // printf("I'm %d\r\n", who);

    double data[SUBDATANUM];

    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // wait for beginning
    while (!thread_begin) {}

    doubleResults[who] = NewSum(data, SUBDATANUM);

    // for (int i = startIndex; i < endIndex; i++)
    // {
    //     doubleResults[who] += sin(sqrt(rawDoubleData[i]));
    // }

    return NULL;
}


int main(int argc, char const *argv[])
{
    int id[MAX_THREADS];

    for (int i = 0; i < MAX_THREADS; i++)
    {
        id[i] = i;
    }
    
    // set stack size to 16M
    // system("ulimit -s 16384");

    // timing way1:
    // timespec start, end;

    // timing way2:
    struct timeval startv, endv;
    struct timezone startz, endz;

    // long t_nsec = 0;
    long t_usec = 0;

    for (int i = 0; i < DATANUM; i++) rawDoubleData[i] = double(i%100);

    double finalResult = 0.0f;

    // clock_gettime(CLOCK_MONOTONIC, &start);
    gettimeofday(&startv, &startz);

    finalResult = NewSum(rawDoubleData, DATANUM);

    // clock_gettime(CLOCK_MONOTONIC, &end);
    gettimeofday(&endv, &endz);

    // t_nsec = end.tv_nsec - start.tv_nsec;
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);

    // printf("test_1: answer = %f, cost = %ld ns\r\n", finalResult, t_nsec);
    printf("single process:\t answer = %f, time = %ld us\r\n", finalResult, t_usec);

    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;

    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    // set stack size to 4 times of original stack size.
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);
    pthread_attr_getstacksize(&attr, &stacksize);
    // printf("%d\r\n", (int)stacksize);

    // create threads
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&tid[i], &attr, threadFunc, &id[i]);
        // usleep(USLEEP_T);
    }
    
    // clock_gettime(CLOCK_MONOTONIC, &start);
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // wait for ending
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    finalResult = 0.0f;
    for (int i = 0; i < MAX_THREADS; i++) finalResult += doubleResults[i];

    // clock_gettime(CLOCK_MONOTONIC, &end);
    gettimeofday(&endv, &endz);

    // t_nsec = end.tv_nsec - start.tv_nsec;
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec)/* - MAX_THREADS*USLEEP_T*/;

    // printf("test_2: answer = %f, cost = %ld ns\r\n", finalResult, t_nsec);
    printf("%d process:\t answer = %f, time = %ld us\r\n", MAX_THREADS, finalResult, t_usec);

    return 0;
}
