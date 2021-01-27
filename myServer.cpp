/* -------------------------------------------
 *
 * File_name: myServer.cpp
 * Create: 2020.12.28
 * Update: 2021.1.27
 * Author: Zhanyu Guo, Peijiang Liu.
 * Note: single-thread || single-pc-multi-thread || multi-PC-multi-thread.
 *
 * -----------------------------------------*/
#define SERVER
#include "mySrvClt.h"

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
        double min_num = log10(sqrt(DATA_MAX));
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_SRV_SUBDATANUM - 1)
            {
                continue;
            }

            if (S_threadResult[i][index[i]] < min_num)
            {
                min_index = i;
                min_num = S_threadResult[i][index[i]];
            }
        }
        S_SRV_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

void merge_0()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
    {
        index[i] = 0;
    }
    int min_index;
    for (int j = 0; j < S_DATANUM; j++)
    {
        double min_num = log10(sqrt(DATA_MAX));
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_SUBDATANUM - 1)
            {
                continue;
            }

            if (S_threadResult0[i][index[i]] < min_num)
            {
                min_index = i;
                min_num = S_threadResult0[i][index[i]];
            }
        }
        S_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

void merge_2()
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
        // printf("%f\r\n", S_sortDoubleData[cnt - 1]);
    }
    if (srv >= S_SRV_DATANUM)
    {
        while (cnt < S_DATANUM)
        {
            S_sortDoubleData[cnt++] = S_CLT_sortDoubleData[clt++];
            // printf("%f\r\n", S_sortDoubleData[cnt - 1]);
        }
    }
    else
    {
        while (cnt < S_DATANUM)
        {
            S_sortDoubleData[cnt++] = S_SRV_sortDoubleData[srv++];
            // printf("%f\r\n", S_sortDoubleData[cnt - 1]);
        }
    }
}

void* fnThreadSum_0(void *arg)
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

void* fnThreadMax_0(void *arg)
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

void* fnThreadSort_0(void* arg)
{
    int who = *(int *)arg;
    double data[S_SUBDATANUM];

    // 索引
    int startIndex = who*S_SUBDATANUM;
    int endIndex = startIndex + S_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];

    while (!thread_begin) {}

    NewSort_0(data, S_SUBDATANUM, S_threadResult0[who]);
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

    NewSort_0(data, S_SRV_SUBDATANUM, S_threadResult[who]);
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

    // 单独生成排序所需要的数据，以免冒泡法排序时耗时过长
    for (int i = 0; i < S_DATANUM; i++)
    {
        S_rawDoubleData[i] = log10(sqrt(fabs(double(rand() + rand() + rand() + rand()))));
        if (!S_rawDoubleData[i]) S_rawDoubleData[i] += 1;
    }
    /* ------------------------------
     *
     * 单机单线程（Server）Code begin
     *
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------
    finalSum = 0.0f;
    gettimeofday(&startv, &startz);
    finalSum = NewSum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Server): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    finalMax = 0.0f;
    gettimeofday(&startv, &startz);
    finalMax = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[MAX](Server): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    gettimeofday(&startv, &startz);
    NewSort_0(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    int checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Single-PC-single-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单机单线程（Server）Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * 单机多线程（Server）Code begin
     *
     * -----------------------------*/

    // 每个线程的ID
    int id[MAX_THREADS];
    // 给每个线程一个ID号地址，保证ID号在创建线程时不会发生冲突
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
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSum_0, &id[i]);

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
    printf("Single-PC-multi-thread[SUM](Server): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax_0, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // 收割
    finalMax = NewMax_2(doubleResults, MAX_THREADS);

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multi-thread[MAX](Server): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort_0, &id[i]);

    gettimeofday(&startv, &startz);
    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge_0();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Single-PC-multi-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 单机多线程（Server）Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * 网络设置（Server）Code begin
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
    printf("# Send: Hello Client!\r\n");

    /* ------------------------------
     *
     * 网络设置（Server）Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * 多机多线程（Server）Code begin
     *
     * -----------------------------*/

    double srvSum = 0.0f;
    double cltSum = 0.0f;
    double srvMax = 0.0f;
    double cltMax = 0.0f;
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
    printf("Srv sum = %f\r\n", srvSum);

    if ((recv_len = recv(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    finalSum = srvSum + cltSum;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-thread[SUM](Server): answer = %lf, time = %ld us\r\n", finalSum, t_usec);
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
    srvMax = NewMax_2(doubleResults, MAX_THREADS);

    if ((recv_len = recv(client_fd, &cltMax, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    printf("Srv max = %f\r\n", srvMax);
    finalMax = srvMax > cltMax ? srvMax : cltMax;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-thread[MAX](Server): answer = %lf, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    /* -----------------------------------------------------------
     * 注意：TCP在单机网上一次最大传输65536字节，即8192个float, 16384个double
     *
     * 在局域网内根据网卡，一次最大传输1500字节，即187个float, 375个double
     * -------------------------------------------------------- */
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSort, &id[i]);

    gettimeofday(&startv, &startz);
    thread_begin = true;

    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    merge();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_SRV_sortDoubleData, S_SRV_DATANUM);
    printf("Srv sort = %d, time = %ld\r\n", checkRlt, t_usec);

    memset(buf, 0, BUF_LEN);
    sprintf(buf, "Come on!");
    if ((send_len = send(client_fd, buf, strlen(buf), 0)) < 0)
    {
        printf("server send failed...\r\n");
        return -1;
    }
    printf("# Send: Come on!\r\n");

    double *p = S_CLT_sortDoubleData;
    for (int i = 0; i < S_TIMES; i++)
    {
        if ((recv_len = recv(client_fd, p, S_ONCE*sizeof(double), 0)) < 0)
        {
            printf("receive client result failed...\r\n");
            return -1;
        }
        // printf("%d, %f\r\n", recv_len, *p);
        p += S_ONCE;
    }
    if (S_LEFT)
    {
        if ((recv_len = recv(client_fd, p, S_LEFT*sizeof(double), 0)) < 0)
        {
            printf("receive result failed...\r\n");
            return -1;
        }
        // printf("%d, %f\r\n", recv_len, *p);
    }

    merge_2();

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Multi-PC-multi-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt,t_usec);
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
