#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
// 需要参考：https://blog.csdn.net/qq_25425023/article/details/72705285
#define MAX_THREADS     64
#define SUBDATANUM      100000
#define DATANUM         (MAX_THREADS*SUBDATANUM)
#define MAX             100000000

double rawDoubleData[DATANUM];
double sortDoubleData[DATANUM];
// double tmpDoubleData[DATANUM];
double rlt[MAX_THREADS][SUBDATANUM];

// double* doubleResults[MAX_THREADS];
bool thread_flag = false;
// double perThreadResult[MAX_THREADS][SUBDATANUM];

// double* NewSort(const double data[], const int len, double result[])
// {
//     double tmp;
//     // for (m = 0; m < len; m++) result[m] = data[m];
//     for (int i = 0; i < len; i++) result[i] = data[i];
    
//     for (int i = 0; i < len; i++)
//     {
//         for (int j = 0; j < len - i - 1; j++)
//         {
//             if (log10(sqrt(result[j])) > log10(sqrt(result[j + 1])))
//             {
//                 tmp = result[j];
//                 result[j] = result[j + 1];
//                 result[j+1] = tmp;
//             }
//         }
//     }

//     return result;
// }

// double NewSort(const double data[], const int len, double result[])
// {
//     double tmp;
//     for (int i = 0; i < len; i++) result[i] = data[i];
    
//     for (int i = 0; i < len; i++)
//     {
//         for (int j = 0; j < len - i - 1; j++)
//         {
//             // if (log10(sqrt(result[j])) > log10(sqrt(result[j + 1])))
//             if (result[j] > result[j + 1])
//             {
//                 tmp = result[j];
//                 result[j] = result[j + 1];
//                 result[j + 1] = tmp;
//             }
//         }
//     }

//     return 1.0;
// }

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

double NewSort(const double data[], const int len, double result[])
{
    int l = 0, r = len - 1;
    for (int i = 0; i < len; i++)
    {
        result[i] = data[i];
    }
    qsort(result, l, r);
}

// void* threadFunc(void* id)
// {
//     int who = *(int *)id;
   
//     int startIndex = who*SUBDATANUM;
//     int endIndex = startIndex + SUBDATANUM;

//     double data[SUBDATANUM];
//     for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];
    
//     while (!thread_flag) {}

//     doubleResults[who] = NewSort(data, SUBDATANUM, perThreadResult[who]);
// }

void* threadFunc(void* id)
{
    int who = *(int *)id;
   
    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;

    double data[SUBDATANUM];
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];
    
    while (!thread_flag) {}

    NewSort(data, SUBDATANUM, rlt[who]);
}

// void merge(double *a, int alen, double *b, int blen, double *result){
//     int i=0,j=0,t=0;
//     while(i<alen&&j<blen){
//         if(a[i]<b[j]){
//             result[t]=a[i];
//             i++;
//         }
//         else {
//             result[t]=b[j];
//             j++;
//         }
//        t++;
//     }
//     while(i<alen){
//         result[t++]=a[i++];
//     }
//     while(j<blen){
//         result[t++]=b[j++];
//     }
// }

void merge()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
    {
        index[i] = 0;
    }
    int min_index;
    for (int j = 0; j < DATANUM; j++)
    {
        double min_num = MAX;
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > SUBDATANUM - 1)
            {
                continue;
            }
            
            if (log10(sqrt(rlt[i][index[i]])) < min_num)
            {
                min_index = i;
                min_num = log10(sqrt(rlt[i][index[i]]));
            }
        }
        sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

int main()
{
    struct timeval startv, endv;
    struct timezone startz, endz;
    long t_usec = 0;

    for (int i = 0; i < DATANUM; i++) rawDoubleData[i] = double(i + 1);
    
    // single-thread
    gettimeofday(&startv, &startz);
    NewSort(rawDoubleData, DATANUM, sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("test_1: cost = %ld us\r\n",t_usec);
    // for (int i = 0; i < DATANUM; i++) printf("%lf\r\n", log10(sqrt(sortDoubleData[i])));

    // multi-threads
    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);

    int id[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) id[i] = i;

    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, threadFunc, (void*)(&id[i]));
    
    gettimeofday(&startv, &startz);
    thread_flag = true;

    // wait for ending
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // for (int i = 0; i < MAX_THREADS; i++)
    // {
    //     double *p = doubleResults[i];
    //     for (int j = 0; j < SUBDATANUM; j++)
    //     {
    //         printf("%lf\r\n", *(p++));
    //     }
        
        
    // }
    
    // double tmp1[2*SUBDATANUM];

    // merge(doubleResults[0], SUBDATANUM, doubleResults[1], SUBDATANUM, tmp1);
    // double *tmp = tmp1;
    // for(int i = 2; i < MAX_THREADS; i++)
    // {
    //     double temp2[(i + 1)*SUBDATANUM];
    //     merge(tmp, i*SUBDATANUM, doubleResults[i], SUBDATANUM, temp2);
    //     tmp = temp2;
    // }
    merge();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("test_2: cost = %ld us\r\n",t_usec);
    // for (int i = 0; i < DATANUM; i++)
    // {
    //     printf("%lf, %d\r\n",sortDoubleData[i], i);
    // }
    
    return 0;
}