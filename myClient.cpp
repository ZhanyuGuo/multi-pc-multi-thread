/* -------------------------------------------
 * 
 * File_name: myClient.cpp
 * Create: 2020.12.28
 * Update: 2021.1.1
 * Author: Zhanyu Guo, Peijiang Liu.
 * Note: Single thread || Multi-PC-multi-thread.
 * 
 * -----------------------------------------*/
#define CLIENT
#include "mySrvClt.h"

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

void merge0()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
    {
        index[i] = 0;
    }
    int min_index;
    for (int j = 0; j < S_DATANUM; j++)
    {
        double min_num = DATA_MAX;
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_SUBDATANUM - 1)
            {
                continue;
            }
            
            if (log10(sqrt(S_threadResult0[i][index[i]])) < min_num)
            {
                min_index = i;
                min_num = log10(sqrt(S_threadResult0[i][index[i]]));
            }
        }
        S_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

void* fnThreadSum0(void *arg)
{
    int who = *(int *)arg;    // 线程ID
    double data[SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewSum(data, SUBDATANUM);

    return NULL;
}

void* fnThreadMax0(void *arg)
{
    int who = *(int *)arg;        // 线程ID
    double data[SUBDATANUM];  // 线程数据
    
    // 索引
    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin) {}

    // 存储结果
    doubleResults[who] = NewMax(data, SUBDATANUM);

    return NULL;
}

void* fnThreadSort0(void* arg)
{
    int who = *(int *)arg;
    double data[S_SUBDATANUM];

    // 索引
    int startIndex = who*S_SUBDATANUM;
    int endIndex = startIndex + S_SUBDATANUM;
    
    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];
    
    while (!thread_begin) {}

    NewSort(data, S_SUBDATANUM, S_threadResult0[who]);
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
     * 单机单线程（Client）Code begin
     *  
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------
    finalSum = 0.0f;
    gettimeofday(&startv, &startz);
    finalSum = NewSum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Client): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    finalMax = 0.0f;
    gettimeofday(&startv, &startz);
    finalMax = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[MAX](Client): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    gettimeofday(&startv, &startz);
    NewSort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    
    int checkRlt = check(S_sortDoubleData, S_DATANUM);

    printf("Single-PC-single-thread[SORT](Client): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单机单线程（Client）Code end
     *  
     * -----------------------------*/

    /* ------------------------------
     *
     * 单机多线程（Client）Code begin
     *  
     * -----------------------------*/

    // 每个线程的ID
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
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSum0, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    finalSum = 0.0f;
    for (int i = 0; i < MAX_THREADS; i++) finalSum += doubleResults[i];

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multi-thread[SUM](Client): answer = %f, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax0, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    finalMax = NewMax2(doubleResults, MAX_THREADS);

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multi-thread[MAX](Client): answer = %f, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------
    
    // ----------------------- SORT begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort0, &id[i]);

    gettimeofday(&startv, &startz);
    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge0();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Single-PC-multi-thread[SORT](Client): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单机多线程（Client）Code end
     *  
     * -----------------------------*/

    /* ------------------------------
     *
     * 网络设置（Client）Code begin
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
     * 网络设置（Client）Code end
     *  
     * -----------------------------*/

    /* ------------------------------
     *
     * 多机多线程（Client）Code begin
     *  
     * -----------------------------*/
    double cltSum = 0.0f;
    double cltMax = 0.0f;
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
    /* -----------------------------------------------------------
     * 注意：TCP在单机网上一次最大传输65536字节，即8192个浮点数
     * 
     * 在局域网内根据网卡，一次最大传输1500字节，即187个浮点数
     * -------------------------------------------------------- */
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort, &id[i]);

    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge();
    checkRlt = check(S_CLT_sortDoubleData, S_CLT_DATANUM);
    printf("Client sort = %d\r\n", checkRlt);

    // if ((send_len = send(client_fd, S_CLT_sortDoubleData, S_CLT_DATANUM*sizeof(double), 0)) < 0)
    // {
    //     printf("send result failed...\r\n");
    //     return -1;
    // }
    memset(buf, 0, BUF_LEN);
    if ((recv_len = recv(client_fd, buf, BUF_LEN, 0)) < 0)
    {
        printf("client recv failed...\r\n");
        return -1;
    }
    printf("# Received: %s\r\n", buf);

    double *p = S_CLT_sortDoubleData;
    for (int i = 0; i < S_TIMES; i++)
    {
        if ((send_len = send(client_fd, p, S_ONCE*sizeof(double), 0)) < 0)
        {
            printf("send result failed...\r\n");
            return -1;
        }
        printf("%d, %lf\r\n", send_len, *p);
        p += S_ONCE;
    }
    if (S_LEFT)
    {
        if ((send_len = send(client_fd, p, S_LEFT*sizeof(double), 0)) < 0)
        {
            printf("send result failed...\r\n");
            return -1;
        }
        printf("%d, %lf\r\n", send_len, *p);
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
