/* -------------------------------------------
 * 
 * File_name: myClient.cpp
 * Create: 2020.12.28
 * Update: 2021.1.1
 * Author: Zhanyu Guo, Peijiang Liu.
 * Note: Single process || Multi-PC-multi-process.
 * 
 * -----------------------------------------*/
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

// 网络相关
#define PORT        8888  // 端口号：8888
#define BUF_LEN     1024  // 缓存长度：1kB
#define SERVER_IP   "127.0.0.1"  // 服务端IP地址

// 线程相关
#define MAX_THREADS         64          // 线程数：64
#define SUBDATANUM          2000000     // 子块数据量：2000000
#define SRV_SUBDATANUM      1000000     // 单PC数据
#define CLT_SUBDATANUM      1000000     // 单PC数据
#define DATANUM             (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量

#define S_SUBDATANUM        2000000         // 由于排序找不到比较快的办法，先减少数据量进行测试
#define S_SRV_SUBDATANUM    1000000          // 单PC小数据量测试
#define S_CLT_SUBDATANUM    1000000          // 单PC小数据量测试
#define S_DATANUM           (S_SUBDATANUM*MAX_THREADS)  // 总数据量(小)：线程数x子块数据量
#define S_CLT_DATANUM       (S_CLT_SUBDATANUM*MAX_THREADS)

#define DATA_MAX            2147483647  // 随机数可能产生的最大值

// 网络收发缓存
char buf[BUF_LEN];

// 计算变量
double rawDoubleData[DATANUM];      // 原始数据
double doubleResults[MAX_THREADS];  // 各线程结果
double finalSum;                    // 求和最终结果
double finalMax;                    // 求最大值最终结果

double S_rawDoubleData[S_DATANUM];
double S_sortDoubleData[S_DATANUM];     // 排序最终结果
double S_CLT_sortDoubleData[S_CLT_DATANUM];     // 排序最终结果
double S_threadResult[MAX_THREADS][S_CLT_SUBDATANUM];

// 标志位
bool thread_begin;          // 线程发令标志

// 不加速版本求和
double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

// 不加速版本求最大值
double NewMax(const double data[], const int len)
{
    double max = log10(sqrt(data[0]/4.0));

    for (int i = 1; i < len; i++) if (log10(sqrt(data[i]/4.0)) > max) max = log10(sqrt(data[i]/4.0));

    return max;
}

// 单纯求最大值
double NewMax2(const double data[], const int len)
{
    double max = data[0];

    for (int i = 1; i < len; i++) if (data[i] > max) max = data[i];

    return max;
}

// 加速版本求和
double NewSumSpeedUp(const double data[], const int len)
{
    double rlt = 0.0f;

    return rlt;
}

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

void merge()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
    {
        index[i] = 0;
    }
    int min_index;
    for (int j = 0; j < S_CLT_DATANUM; j++)
    {
        double min_num = DATA_MAX;
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_CLT_SUBDATANUM - 1)
            {
                continue;
            }
            
            if (log10(sqrt(S_threadResult[i][index[i]])) < min_num)
            {
                min_index = i;
                min_num = log10(sqrt(S_threadResult[i][index[i]]));
            }
        }
        S_CLT_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

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
            return 0;
        }
    }

    return 1;
}

void* fnThreadSum(void *arg)
{
    int who = *(int *)arg;        // 线程ID
    double data[CLT_SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*CLT_SUBDATANUM + SRV_SUBDATANUM*MAX_THREADS;
    int endIndex = startIndex + CLT_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewSum(data, CLT_SUBDATANUM);
    
    return NULL;
}

void* fnThreadMax(void *arg)
{
    int who = *(int *)arg;        // 线程ID
    double data[CLT_SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*CLT_SUBDATANUM + SRV_SUBDATANUM*MAX_THREADS;
    int endIndex = startIndex + CLT_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewMax(data, CLT_SUBDATANUM);

    return NULL;
}

void* fnThreadSort(void* arg)
{
    int who = *(int *)arg;
    double data[S_CLT_SUBDATANUM];

    // 索引
    int startIndex = who*S_CLT_SUBDATANUM + S_SRV_SUBDATANUM*MAX_THREADS;
    int endIndex = startIndex + S_CLT_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];
    
    while (!thread_begin) {}

    NewSort(data, S_CLT_SUBDATANUM, S_threadResult[who]);
}

int main(int argc, char const *argv[])
{
    // 计时相关
    struct timeval startv, endv;
    struct timezone startz, endz;
    long t_usec = 0;  // 时间间隔，单位us

    // 初始化数据
    srand(1);
    // for (int i = 0; i < DATANUM; i++) rawDoubleData[i] = double(i + 1);
    for (int i = 0; i < DATANUM; i++) 
    {
        rawDoubleData[i] = fabs(double(rand() + rand() + rand() + rand()));
        if (!rawDoubleData[i]) rawDoubleData[i] += 1;
    }
    for (int i = 0; i < S_DATANUM; i++)
    {
        S_rawDoubleData[i] = fabs(double(rand() + rand() + rand() + rand()));
        if (!S_rawDoubleData[i]) S_rawDoubleData[i] += 1;
    }
    
    /* ------------------------------
     *
     * 单线程（Server）Code begin
     *  
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------
    finalSum = 0.0f;
    gettimeofday(&startv, &startz);
    finalSum = NewSum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single Process[SUM](Client): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    finalMax = 0.0f;
    gettimeofday(&startv, &startz);
    finalMax = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single Process[MAX](Client): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    gettimeofday(&startv, &startz);
    NewSort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    
    int checkRlt = check(S_sortDoubleData, S_DATANUM);

    printf("Single Process[SORT](Client): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单线程（Server）Code end
     *  
     * -----------------------------*/

    // Client socket描述符
    int client_fd;
    // My address, remote address
    struct sockaddr_in remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len;

    // 设置地址
    memset(&remote_addr,0,sizeof(remote_addr));         // reset
    remote_addr.sin_family = AF_INET;                   // IPV4
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Server IP
    remote_addr.sin_port = htons(PORT);                 // Port

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create client_fd failed...\r\n");
        return -1;
    }

    if ((ret = connect(client_fd, (struct sockaddr*)&remote_addr, sizeof(remote_addr))) < 0)
    {
        printf("connect failed...\r\n");
        return -1;
    }
    printf("Connect to server!\r\n");
    
    if ((recv_len = recv(client_fd, buf, BUF_LEN, 0)) < 0)
    {
        printf("client recv failed...\r\n");
        return -1;
    }
    printf("# Received: %s\r\n", buf);

    /* ------------------------------
     *
     * 多机多线程（Client）Code begin
     *  
     * -----------------------------*/
    double cltSum = 0.0f;
    double cltMax = 0.0f;

    // 给每个线程初始化ID
    int id[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) id[i] = i;

    // 多线程相关
    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;

    // 更改栈的大小
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);
    
    // ------------------------ SUM begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSum, &id[i]);

    // 给多个线程同时发令
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    for (int i = 0; i < MAX_THREADS; i++) cltSum += doubleResults[i];
    printf("Client sum = %f\r\n", cltSum);
    
    if ((send_len = send(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("send result failed...\r\n");
        return -1;
    }
    printf("Client sum result send successfully!\r\n");
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax, &id[i]);

    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    cltMax = NewMax2(doubleResults, MAX_THREADS);
    printf("Client max = %f\r\n", cltMax);
    
    if ((send_len = send(client_fd, &cltMax, sizeof(cltMax), 0)) < 0)
    {
        printf("send result failed...\r\n");
        return -1;
    }
    printf("Client max result send successfully!\r\n");
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort, &id[i]);

    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge();
    checkRlt = check(S_CLT_sortDoubleData, S_CLT_DATANUM);
    printf("Client sort = %d\r\n", checkRlt);
    if ((send_len = send(client_fd, S_CLT_sortDoubleData, S_CLT_DATANUM*sizeof(double), 0)) < 0)
    {
        printf("send result failed...\r\n");
        return -1;
    }
    printf("Client sort result send successfully!\r\n");
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 多机多线程（Client）Code end
     *  
     * -----------------------------*/

    close(client_fd);
    return 0;
}
