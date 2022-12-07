#define CLIENT
#include "mySrvClt.hpp"

void *fnThreadSum(void *arg)
{
    int who = *(int *)arg;   // 线程ID
    double data[SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;

    // 数据转移
    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = sum(data, SUBDATANUM);
    // doubleResults[who] = sumSSE(data, SUBDATANUM);  // USE SSE

    return NULL;
}

void *fnThreadMax(void *arg)
{
    int who = *(int *)arg;   // 线程ID
    double data[SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = max(data, SUBDATANUM);

    return NULL;
}

void *fnThreadSort(void *arg)
{
    int who = *(int *)arg;     // 线程ID
    double data[S_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * S_SUBDATANUM;
    int endIndex = startIndex + S_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = S_rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    sort(data, S_SUBDATANUM, S_threadResult[who]);
}

void *fnThreadSum_multiPC(void *arg)
{
    int who = *(int *)arg;       // 线程ID
    double data[CLT_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * CLT_SUBDATANUM + SRV_SUBDATANUM * MAX_THREADS;
    int endIndex = startIndex + CLT_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = sum(data, CLT_SUBDATANUM);

    return NULL;
}

void *fnThreadMax_multiPC(void *arg)
{
    int who = *(int *)arg;       // 线程ID
    double data[CLT_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * CLT_SUBDATANUM + SRV_SUBDATANUM * MAX_THREADS;
    int endIndex = startIndex + CLT_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = max(data, CLT_SUBDATANUM);

    return NULL;
}

void *fnThreadSort_multiPC(void *arg)
{
    int who = *(int *)arg;         // 线程ID
    double data[S_CLT_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * S_CLT_SUBDATANUM + S_SRV_SUBDATANUM * MAX_THREADS;
    int endIndex = startIndex + S_CLT_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = S_rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    sort(data, S_CLT_SUBDATANUM, S_threadResult0[who]);
}

void merge()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
        index[i] = 0;

    int min_index;
    for (int j = 0; j < S_DATANUM; j++)
    {
        double min_num = log10(sqrt(DATA_MAX));
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_SUBDATANUM - 1)
                continue;

            if (S_threadResult[i][index[i]] < min_num)
            {
                min_index = i;
                min_num = S_threadResult[i][index[i]];
            }
        }
        S_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

void mergeClt()
{
    int index[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++)
        index[i] = 0;

    int min_index;
    for (int j = 0; j < S_CLT_DATANUM; j++)
    {
        double min_num = log10(sqrt(DATA_MAX));
        for (int i = 0; i < MAX_THREADS; i++)
        {
            if (index[i] > S_CLT_SUBDATANUM - 1)
                continue;

            if (S_threadResult[i][index[i]] < min_num)
            {
                min_index = i;
                min_num = S_threadResult0[i][index[i]];
            }
        }
        S_CLT_sortDoubleData[j] = min_num;
        index[min_index]++;
    }
}

int main(int argc, char const *argv[])
{
    // 计时相关
    struct timeval startv, endv;
    struct timezone startz, endz;

    // 时间间隔，单位us
    long t_usec = 0;

    // 初始化数据
    srand(1);
    for (int i = 0; i < DATANUM; i++)
    {
        rawDoubleData[i] = fabs(double(rand()));
        if (!rawDoubleData[i])
            rawDoubleData[i] += 1;
    }

    /* ------------------------------
     *
     * 单机单线程（Client）
     *
     * -----------------------------*/

    // ------------------------ SUM ------------------------

    finalSum = 0.0;
    gettimeofday(&startv, &startz);
    finalSum = sum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Client): answer = %lf, time = %ld us\n", finalSum, t_usec);

    // ------------------------ MAX ------------------------

    finalMax = 0.0;
    gettimeofday(&startv, &startz);
    finalMax = max(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[MAX](Client): answer = %lf, time = %ld us\n", finalMax, t_usec);

    // ----------------------- SORT ------------------------

    gettimeofday(&startv, &startz);
    sort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SOT](Client): answer = %d, time = %ld us\n", isSorted(S_sortDoubleData, S_DATANUM), t_usec);

    cout << endl;
    /* ------------------------------
     *
     * 单机多线程（Client）
     *
     * -----------------------------*/

    // 每个线程的ID
    int id[MAX_THREADS];

    // 给每个线程一个ID号地址，保证ID号在创建线程时不会发生冲突
    for (int i = 0; i < MAX_THREADS; i++)
        id[i] = i;

    // 多线程相关
    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;

    // 更改栈的大小
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);

    // ------------------------ SUM ------------------------

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadSum, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    finalSum = 0.0f;
    for (int i = 0; i < MAX_THREADS; i++)
        finalSum += doubleResults[i];

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multip-thread[SUM](Client): answer = %lf, time = %ld us\n", finalSum, t_usec);

    // ------------------------ MAX ------------------------

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadMax, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    finalMax = maxMerge(doubleResults, MAX_THREADS);

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multip-thread[MAX](Client): answer = %lf, time = %ld us\n", finalMax, t_usec);

    // ----------------------- SORT ------------------------

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadSort, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    merge();

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multip-thread[SOT](Client): answer = %d, time = %ld us\n", isSorted(S_sortDoubleData, S_DATANUM), t_usec);

    cout << endl;
    /* ------------------------------
     *
     * 网络设置（Client）
     *
     * -----------------------------*/

    // Client socket描述符
    int client_fd;
    // My address, remote address
    struct sockaddr_in remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len;

    // 设置地址
    memset(&remote_addr, 0, sizeof(remote_addr));       // reset
    remote_addr.sin_family = AF_INET;                   // IPV4
    remote_addr.sin_addr.s_addr = inet_addr(SERVER_IP); // Server IP
    remote_addr.sin_port = htons(PORT);                 // Port

    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create client_fd failed...\r\n");
        return -1;
    }

    if ((ret = connect(client_fd, (struct sockaddr *)&remote_addr, sizeof(remote_addr))) < 0)
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
     * 多机多线程（Client）
     *
     * -----------------------------*/

    // ------------------------ SUM ------------------------

    double cltSum = 0.0;

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadSum_multiPC, &id[i]);

    // 给多个线程同时发令
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    for (int i = 0; i < MAX_THREADS; i++)
        cltSum += doubleResults[i];
    printf("Client sum = %lf\n", cltSum);

    if ((send_len = send(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("send result failed...\n");
        return -1;
    }
    printf("Client sum result send successfully!\n");

    // ------------------------ MAX ------------------------

    double cltMax = 0.0;

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadMax_multiPC, &id[i]);

    // 给多个线程同时发令
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    cltMax = maxMerge(doubleResults, MAX_THREADS);
    printf("Client max = %lf\n", cltMax);

    if ((send_len = send(client_fd, &cltMax, sizeof(cltMax), 0)) < 0)
    {
        printf("send result failed...\n");
        return -1;
    }
    printf("Client max result send successfully!\n");

    // ----------------------- SORT ------------------------
    /* -----------------------------------------------------------
     * 注意：TCP在单机网上一次最大传输65536字节，即8192个double, 16384个float
     *
     * 在局域网内根据网卡，一次最大传输1500字节，即187个double, 375个float
     * -------------------------------------------------------- */

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadSort_multiPC, &id[i]);

    // 给多个线程同时发令
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    mergeClt();
    printf("Client sort = %d\n", isSorted(S_CLT_sortDoubleData, S_CLT_DATANUM));

    memset(buf, 0, BUF_LEN);
    if ((recv_len = recv(client_fd, buf, BUF_LEN, 0)) < 0)
    {
        printf("client recv failed...\n");
        return -1;
    }
    printf("# Received: %s\r\n", buf);

    double *p = S_CLT_sortDoubleData;
    for (int i = 0; i < S_TIMES; i++)
    {
        if ((send_len = send(client_fd, p, S_ONCE * sizeof(double), 0)) < 0)
        {
            printf("send result failed...\n");
            return -1;
        }
        p += S_ONCE;
    }
    if (S_LEFT)
    {
        if ((send_len = send(client_fd, p, S_LEFT * sizeof(double), 0)) < 0)
        {
            printf("send result failed...\n");
            return -1;
        }
    }

    printf("Client sort result send successfully!\n");

    close(client_fd);

    return 0;
}
