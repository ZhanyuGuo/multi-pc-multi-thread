#define SERVER
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
    double data[SRV_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = sum(data, SRV_SUBDATANUM);

    return NULL;
}

void *fnThreadMax_multiPC(void *arg)
{
    int who = *(int *)arg;       // 线程ID
    double data[SRV_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    // 存储结果
    doubleResults[who] = max(data, SRV_SUBDATANUM);

    return NULL;
}

void *fnThreadSort_multiPC(void *arg)
{
    int who = *(int *)arg;         // 线程ID
    double data[S_SRV_SUBDATANUM]; // 线程数据

    // 索引
    int startIndex = who * S_SRV_SUBDATANUM;
    int endIndex = startIndex + S_SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++)
        data[j] = S_rawDoubleData[i];

    // 等待发令
    while (!thread_begin)
        ;

    sort(data, S_SRV_SUBDATANUM, S_threadResult0[who]);
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

void merge_multiPC()
{
    int srv = 0, clt = 0, cnt = 0;

    while (srv < S_SRV_DATANUM && clt < S_CLT_DATANUM)
        if (S_SRV_sortDoubleData[srv] < S_CLT_sortDoubleData[clt])
            S_sortDoubleData[cnt++] = S_SRV_sortDoubleData[srv++];
        else
            S_sortDoubleData[cnt++] = S_CLT_sortDoubleData[clt++];
    if (srv >= S_SRV_DATANUM)
        while (cnt < S_DATANUM)
            S_sortDoubleData[cnt++] = S_CLT_sortDoubleData[clt++];
    else
        while (cnt < S_DATANUM)
            S_sortDoubleData[cnt++] = S_SRV_sortDoubleData[srv++];
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
     * 单机单线程（Server）
     *
     * -----------------------------*/

    // ------------------------ SUM ------------------------

    finalSum = 0.0;
    gettimeofday(&startv, &startz);
    finalSum = sum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Server): answer = %lf, time = %ld us\n", finalSum, t_usec);

    // ------------------------ MAX ------------------------

    finalMax = 0.0;
    gettimeofday(&startv, &startz);
    finalMax = max(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[MAX](Server): answer = %lf, time = %ld us\n", finalMax, t_usec);

    // ----------------------- SORT ------------------------

    gettimeofday(&startv, &startz);
    sort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SOT](Server): answer = %d, time = %ld us\n", isSorted(S_sortDoubleData, S_DATANUM), t_usec);

    cout << endl;
    /* ------------------------------
     *
     * 单机多线程（Server）
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
    printf("Single-PC-multip-thread[SUM](Server): answer = %lf, time = %ld us\n", finalSum, t_usec);

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
    printf("Single-PC-multip-thread[MAX](Server): answer = %lf, time = %ld us\n", finalMax, t_usec);

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
    printf("Single-PC-multip-thread[SOT](Server): answer = %d, time = %ld us\n", isSorted(S_sortDoubleData, S_DATANUM), t_usec);

    cout << endl;
    /* ------------------------------
     *
     * 网络设置（Server）
     *
     * -----------------------------*/

    // Server, Client socket描述符
    int server_fd, client_fd;
    // My address, remote address
    struct sockaddr_in my_addr, remote_addr;
    // 一些网络收发相关变量
    int ret, recv_len, send_len, sin_size;

    // 设置地址
    memset(&my_addr, 0, sizeof(my_addr)); // reset
    my_addr.sin_family = AF_INET;         // IPV4
    my_addr.sin_addr.s_addr = INADDR_ANY; // Local IP
    my_addr.sin_port = htons(PORT);       // Port

    if ((server_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("create server_fd failed...\r\n");
        return -1;
    }

    if ((ret = bind(server_fd, (struct sockaddr *)&my_addr, sizeof(my_addr))) < 0)
    {
        printf("bind server_fd failed...\r\n");
        return -1;
    }

    // 等待连接
    listen(server_fd, 5);

    // 只针对一个client的情况
    sin_size = sizeof(struct sockaddr_in);
    if ((client_fd = accept(server_fd, (struct sockaddr *)&remote_addr, (socklen_t *)&sin_size)) < 0)
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
     * 多机多线程（Server）
     *
     * -----------------------------*/

    // ------------------------ SUM ------------------------

    double srvSum = 0.0;
    double cltSum = 0.0;

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadSum_multiPC, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    for (int i = 0; i < MAX_THREADS; i++)
        srvSum += doubleResults[i];
    printf("Server sum = %lf\n", srvSum);

    if ((recv_len = recv(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\n");
        return -1;
    }
    finalSum = srvSum + cltSum;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multip-PC-multip-thread[SUM](Server): answer = %lf, time = %ld us\n", finalSum, t_usec);

    // ------------------------ MAX ------------------------

    double srvMax = 0.0;
    double cltMax = 0.0;

    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadMax_multiPC, &id[i]);

    // 给多个线程同时发令
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    // 收割
    srvMax = maxMerge(doubleResults, MAX_THREADS);
    printf("Server max = %lf\n", srvMax);

    if ((recv_len = recv(client_fd, &cltMax, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    finalMax = srvMax > cltMax ? srvMax : cltMax;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multip-PC-multip-thread[MAX](Server): answer = %lf, time = %ld us\r\n", finalMax, t_usec);

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
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // 等待线程运行结束
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    mergeClt();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Server sort = %d, time = %ld\n", isSorted(S_SRV_sortDoubleData, S_SRV_DATANUM), t_usec);

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
        if ((recv_len = recv(client_fd, p, S_ONCE * sizeof(double), 0)) < 0)
        {
            printf("receive client result failed...\r\n");
            return -1;
        }
        p += S_ONCE;
    }
    if (S_LEFT)
    {
        if ((recv_len = recv(client_fd, p, S_LEFT * sizeof(double), 0)) < 0)
        {
            printf("receive result failed...\r\n");
            return -1;
        }
    }

    merge_multiPC();
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-thread[SOT](Server): answer = %d, time = %ld us\n", isSorted(S_sortDoubleData, S_DATANUM), t_usec);

    close(client_fd);
    close(server_fd);

    return 0;
}
