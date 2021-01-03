/* -------------------------------------------
 * 
 * File_name: myServer.cpp
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

// 线程相关
#define MAX_THREADS     64          // 线程数：64
#define SUBDATANUM      2000000     // 子块数据量：2000000
#define SRV_SUBDATANUM  1000000     // 单PC数据
#define DATANUM         (SUBDATANUM*MAX_THREADS)  // 总数据量：线程数x子块数据量

#define S_SUBDATANUM        2000000         // 由于排序找不到比较快的办法，先减少数据量进行测试
#define S_SRV_SUBDATANUM    1000000          // 单PC小数据量测试
#define S_CLT_SUBDATANUM    1000000          // 单PC小数据量测试
#define S_DATANUM           (S_SUBDATANUM*MAX_THREADS)  // 总数据量(小)：线程数x子块数据量
#define S_SRV_DATANUM       (S_SRV_SUBDATANUM*MAX_THREADS)
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
double S_SRV_sortDoubleData[S_SRV_DATANUM];     // 排序最终结果
double S_CLT_sortDoubleData[S_CLT_DATANUM];     // 排序最终结果
double S_threadResult[MAX_THREADS][S_SRV_SUBDATANUM];

// 标志位
bool thread_begin;          // 线程发令标志

// 不加速版本求和
double NewSum(const double data[], const int len)
{
    double rlt = 0.0f;

    for (int i = 0; i < len; i++) rlt += log10(sqrt(data[i]/4.0));
    
    return rlt;
}

// 不加速版本求最大值：增大消耗
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
    for (int j = 0; j < S_SRV_DATANUM; j++)
    {
        double min_num = DATA_MAX;
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_SRV_SUBDATANUM - 1)
            {
                continue;
            }
            
            if (log10(sqrt(S_threadResult[i][index[i]])) < min_num)
            {
                min_index = i;
                min_num = log10(sqrt(S_threadResult[i][index[i]]));
            }
        }
        S_SRV_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

void merge2()
{
    int srv = 0, clt = 0, cnt = 0;
    while (srv < S_SRV_DATANUM && clt < S_CLT_DATANUM)
    {
        if (S_SRV_sortDoubleData[srv] < S_CLT_sortDoubleData[clt])
        {
            S_sortDoubleData[cnt++] = S_SRV_sortDoubleData[srv++];
        }
        else
        {
            S_sortDoubleData[cnt++] = S_CLT_sortDoubleData[clt++];
        }
        // printf("%lf\r\n", S_sortDoubleData[cnt - 1]);
    }
    if (srv >= S_SRV_DATANUM)
    {
        while (cnt < S_DATANUM)
        {
            S_sortDoubleData[cnt++] = S_CLT_sortDoubleData[clt++];
            // printf("%lf\r\n", S_sortDoubleData[cnt - 1]);
        }
    }
    else
    {
        while (cnt < S_DATANUM)
        {
            S_sortDoubleData[cnt++] = S_SRV_sortDoubleData[srv++];
            // printf("%lf\r\n", S_sortDoubleData[cnt - 1]);
        }
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
            printf("i = %d, %lf, %lf\r\n", i, data[i], data[i + 1]);
            return 0;
        }
    }

    return 1;
}

void* fnThreadSum(void *arg)
{
    int who = *(int *)arg;        // 线程ID
    double data[SRV_SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewSum(data, SRV_SUBDATANUM);

    return NULL;
}

void* fnThreadMax(void *arg)
{
    int who = *(int *)arg;        // 线程ID
    double data[SRV_SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewMax(data, SRV_SUBDATANUM);

    return NULL;
}

void* fnThreadSort(void* arg)
{
    int who = *(int *)arg;
    double data[S_SRV_SUBDATANUM];

    // 索引
    int startIndex = who*S_SRV_SUBDATANUM;
    int endIndex = startIndex + S_SRV_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];
    
    while (!thread_begin) {}

    NewSort(data, S_SRV_SUBDATANUM, S_threadResult[who]);
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
    printf("Single Process[SUM](Server): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    finalMax = 0.0f;
    gettimeofday(&startv, &startz);
    finalMax = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single Process[MAX](Server): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    gettimeofday(&startv, &startz);
    NewSort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    
    int checkRlt = check(S_sortDoubleData, S_DATANUM);

    printf("Single Process[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单线程（Server）Code end
     *  
     * -----------------------------*/

    // Server, Client socket描述符
    int server_fd, client_fd;
    // My address, remote address
    struct sockaddr_in my_addr, remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len, sin_size;

    // 设置地址
    memset(&my_addr, 0, sizeof(my_addr));   // reset
    my_addr.sin_family = AF_INET;           // IPV4
    my_addr.sin_addr.s_addr = INADDR_ANY;   // Local IP
    my_addr.sin_port = htons(PORT);         // Port

    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create server_fd failed...\r\n");
        return -1;
    }

    if ((ret = bind(server_fd,(struct sockaddr *)&my_addr,sizeof(my_addr))) < 0)
    {
        printf("bind server_fd failed...\r\n");
        return -1;
    }

    // 等待连接
    listen(server_fd, 5);

    // 只针对一个client的情况
    sin_size = sizeof(struct sockaddr_in);
    if ((client_fd = accept(server_fd, (struct sockaddr*)&remote_addr, (socklen_t *)&sin_size)) < 0)
    {
        printf("accept connection failed...\r\n");
        return -1;
    }
    printf("# Accept client %s\r\n", inet_ntoa(remote_addr.sin_addr));
    
    // 对Client的应答
    memset(buf, 0, BUF_LEN);
    sprintf(buf, "Hello Client!");
    if ((send_len = send(client_fd, buf, strlen(buf), 0)) < 0)
    {
        printf("server send failed...\r\n");
        return -1;
    }

    /* ------------------------------
     *
     * 多机多线程（Server）Code begin
     *  
     * -----------------------------*/
    double srvSum = 0.0f;
    double cltSum = 0.0f;
    double srvMax = 0.0f;
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
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    for (int i = 0; i < MAX_THREADS; i++) srvSum += doubleResults[i];

    if ((recv_len = recv(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    finalSum = srvSum + cltSum;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-Process[SUM](Server): answer = %f, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    srvMax = NewMax2(doubleResults, MAX_THREADS);

    if ((recv_len = recv(client_fd, &cltMax, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    const int tmp_len = 2;
    double temp[tmp_len];
    temp[0] = srvMax;
    temp[1] = cltMax;
    printf("Srv max = %lf\r\n", srvMax);
    finalMax = NewMax2(temp, tmp_len);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-Process[MAX](Server): answer = %f, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------
    
    // ----------------------- SORT begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort, &id[i]);

    gettimeofday(&startv, &startz);
    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge();
    checkRlt = check(S_SRV_sortDoubleData, S_SRV_DATANUM);
    printf("Srv sort = %d\r\n", checkRlt);
    if ((recv_len = recv(client_fd, S_CLT_sortDoubleData, S_CLT_DATANUM*sizeof(double), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }

    // printf("%ld\r\n", sizeof(S_CLT_sortDoubleData)/sizeof(double));
    // for (int i = 0; i < S_CLT_DATANUM; i++)
    // {
    //     printf("%lf\r\n", S_CLT_sortDoubleData[i]);
    // }
    
    merge2();
    // for (int i = 0; i < S_DATANUM; i++)
    // {
    //     printf("%lf\r\n", S_sortDoubleData[i]);
    // }
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Multi-PC-multi-Process[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 多机多线程（Server）Code end
     *  
     * -----------------------------*/

    close(client_fd);
    close(server_fd);
    return 0;
}
