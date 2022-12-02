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
        // rawDoubleData[i] = double(i + 1);
        rawDoubleData[i] = fabs(double(rand() + rand() + rand() + rand()));
        if (!rawDoubleData[i])
            rawDoubleData[i] += 1;
    }

    /* ------------------------------
     *
     * 单机单线程（Client）
     *
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------

    finalSum = 0.0;
    gettimeofday(&startv, &startz);
    finalSum = sum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Client): answer = %lf, time = %ld us\r\n", finalSum, t_usec);

    // ------------------------  SUM end -------------------------

    // ------------------------ MAX begin ------------------------
    // TODO
    // ------------------------  MAX end -------------------------

    // ----------------------- SORT begin ------------------------
    // TODO
    // ------------------------ SORT end -------------------------

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

    // ------------------------ SUM begin ------------------------

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
    printf("Single-PC-multip-thread[SUM](Client): answer = %lf, time = %ld us\r\n", finalSum, t_usec);

    // ------------------------  SUM end -------------------------

    // ------------------------ MAX begin ------------------------
    // TODO
    // ------------------------  MAX end -------------------------

    // ----------------------- SORT begin ------------------------
    // TODO
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * 网络设置（Client）
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
     * 多机多线程（Client）
     *
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------

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
    printf("Server sum = %lf\r\n", srvSum);

    if ((recv_len = recv(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    finalSum = srvSum + cltSum;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multip-PC-multip-thread[SUM](Server): answer = %lf, time = %ld us\r\n", finalSum, t_usec);

    // ------------------------  SUM end -------------------------

    // ------------------------ MAX begin ------------------------
    // TODO
    // ------------------------  MAX end -------------------------

    // ----------------------- SORT begin ------------------------
    // TODO
    // ------------------------ SORT end -------------------------

    close(client_fd);
    close(server_fd);

    return 0;
}
