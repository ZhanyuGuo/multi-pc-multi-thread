/* -------------------------------------------
 * 
 * File_name: myServer.cpp
 * Create: 2020.12.28
 * Update: 2021.1.1 
 * Author: Zhanyu Guo, Peijiang Liu.
 * Note: Single process || Multi-PC-multi-process.
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
    printf("# Send: Hello Client!\r\n");

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
    /* -----------------------------------------------------------
     * 注意：TCP在单机网上一次最大传输65536字节，即8192个浮点数
     * 
     * 在局域网内根据网卡，一次最大传输1500字节，即187个浮点数
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
    // if ((recv_len = recv(client_fd, S_CLT_sortDoubleData, S_CLT_DATANUM*sizeof(double), 0)) < 0)
    // {
    //     printf("receive client result failed...\r\n");
    //     return -1;
    // }

    // for (int i = 0; i < S_CLT_DATANUM; i++)
    // {
    //     if (!S_CLT_sortDoubleData[i])
    //     {
    //         printf("%d\r\n", i);
    //         break;
    //     }
        
    //     printf("%lf\r\n", S_CLT_sortDoubleData[i]);
    // }

    double *p = S_CLT_sortDoubleData;
    for (int i = 0; i < S_TIMES; i++)
    {
        if ((recv_len = recv(client_fd, p, S_ONCE*sizeof(double), 0)) < 0)
        {
            printf("receive client result failed...\r\n");
            return -1;
        }
        // printf("%d, %lf\r\n", recv_len, *p);
        p += S_ONCE;
    }
    if (S_LEFT)
    {
        if ((recv_len = recv(client_fd, p, S_LEFT*sizeof(double), 0)) < 0)
        {
            printf("receive result failed...\r\n");
            return -1;
        }
        // printf("%d, %lf\r\n", recv_len, *p);
    }
    
    merge2();
    // for (int i = 0; i < 16450; i++)
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
