/* -------------------------------------------
 *
 * File_name: myServer.cpp
 * Create: 2020.12.28
 * Update: 2021.1.6
 * Author: Zhanyu Guo, Peijiang Liu.
 * Note: Single thread || Multi-PC-multi-thread.
 *
 * -----------------------------------------*/
#define SERVER
#include "mySrvClt copy.h"

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
        double min_num = sqrtf(sqrtf(DATA_MAX));
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
        double min_num = sqrtf(sqrtf(DATA_MAX));
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

void* fnThreadSum0(void *arg)
{
    int who = *(int *)arg;    // ??????ID
    double data[SUBDATANUM];  // ????????????

    // ??????
    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // ????????????
    while (!thread_begin) {}

    // ????????????
    doubleResults[who] = NewSumSSE(data, SUBDATANUM);

    return NULL;
}

void* fnThreadMax0(void *arg)
{
    int who = *(int *)arg;        // ??????ID
    double data[SUBDATANUM];  // ????????????

    // ??????
    int startIndex = who*SUBDATANUM;
    int endIndex = startIndex + SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // ????????????
    while (!thread_begin) {}

    // ????????????
    doubleResults[who] = NewMax(data, SUBDATANUM);

    return NULL;
}

void* fnThreadSort0(void* arg)
{
    int who = *(int *)arg;
    double data[S_SUBDATANUM];

    // ??????
    int startIndex = who*S_SUBDATANUM;
    int endIndex = startIndex + S_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];

    while (!thread_begin) {}

    NewSort(data, S_SUBDATANUM, S_threadResult0[who]);
}

void* fnThreadSum(void *arg)
{
    int who = *(int *)arg;        // ??????ID
    double data[SRV_SUBDATANUM];  // ????????????

    // ??????
    int startIndex = who*SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // ????????????
    while (!thread_begin) {}

    // ????????????
    doubleResults[who] = NewSumSSE(data, SRV_SUBDATANUM);

    return NULL;
}

void* fnThreadMax(void *arg)
{
    int who = *(int *)arg;        // ??????ID
    double data[SRV_SUBDATANUM];  // ????????????

    // ??????
    int startIndex = who*SRV_SUBDATANUM;
    int endIndex = startIndex + SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = rawDoubleData[i];

    // ????????????
    while (!thread_begin) {}

    // ????????????
    doubleResults[who] = NewMax(data, SRV_SUBDATANUM);

    return NULL;
}

void* fnThreadSort(void* arg)
{
    int who = *(int *)arg;
    double data[S_SRV_SUBDATANUM];

    // ??????
    int startIndex = who*S_SRV_SUBDATANUM;
    int endIndex = startIndex + S_SRV_SUBDATANUM;

    for (int i = startIndex, j = 0; i < endIndex; i++, j++) data[j] = S_rawDoubleData[i];

    while (!thread_begin) {}

    NewSort(data, S_SRV_SUBDATANUM, S_threadResult[who]);
}


int main(int argc, char const *argv[])
{
    // ????????????
    struct timeval startv, endv;
    struct timezone startz, endz;
    long t_usec = 0;  // ?????????????????????us

    // ???????????????
    srand(1);
    for (int i = 0; i < DATANUM; i++)
    {
        rawDoubleData[i] = fabsf(double(rand() + rand() + rand() + rand()));
        if (!rawDoubleData[i]) rawDoubleData[i] += 1;
    }
    for (int i = 0; i < S_DATANUM; i++)
    {
        S_rawDoubleData[i] = sqrtf(sqrtf(fabsf(double(rand() + rand() + rand() + rand()))));
        if (!S_rawDoubleData[i]) S_rawDoubleData[i] += 1;
    }
    /* ------------------------------
     *
     * ??????????????????Server???Code begin
     *
     * -----------------------------*/

    // ------------------------ SUM begin ------------------------
    finalSum = 0.0f;
    gettimeofday(&startv, &startz);
    finalSum = NewSum(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[SUM](Server): answer = %f, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    finalMax = 0.0f;
    gettimeofday(&startv, &startz);
    finalMax = NewMax(rawDoubleData, DATANUM);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-single-thread[MAX](Server): answer = %f, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    gettimeofday(&startv, &startz);
    NewSort(S_rawDoubleData, S_DATANUM, S_sortDoubleData);
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);

    int checkRlt = check(S_sortDoubleData, S_DATANUM);

    printf("Single-PC-single-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * ??????????????????Server???Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * ??????????????????Server???Code begin
     *
     * -----------------------------*/

    // ???????????????ID
    int id[MAX_THREADS];
    for (int i = 0; i < MAX_THREADS; i++) id[i] = i;

    // ???????????????
    pthread_t tid[MAX_THREADS];
    pthread_attr_t attr;
    size_t stacksize;

    // ??????????????????
    pthread_attr_init(&attr);
    pthread_attr_getstacksize(&attr, &stacksize);
    stacksize *= 4;
    pthread_attr_setstacksize(&attr, stacksize);

    // ------------------------ SUM begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSum0, &id[i]);

    // ???????????????????????????
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // ????????????????????????
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // ??????
    finalSum = 0.0f;
    for (int i = 0; i < MAX_THREADS; i++) finalSum += doubleResults[i];

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multi-thread[SUM](Server): answer = %f, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax0, &id[i]);

    // ???????????????????????????
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // ????????????????????????
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // ??????
    finalMax = NewMax2(doubleResults, MAX_THREADS);

    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Single-PC-multi-thread[MAX](Server): answer = %f, time = %ld us\r\n", finalMax, t_usec);
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
    printf("Single-PC-multi-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * ??????????????????Server???Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * ???????????????Server???Code begin
     *
     * -----------------------------*/

    // Server, Client socket?????????
    int server_fd, client_fd;
    // My address, remote address
    struct sockaddr_in my_addr, remote_addr;
    // ??????????????????????????????
    int ret, recv_len, send_len, sin_size;

    // ????????????
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

    // ????????????
    listen(server_fd, 5);

    // ???????????????client?????????
    sin_size = sizeof(struct sockaddr_in);
    if ((client_fd = accept(server_fd, (struct sockaddr*)&remote_addr, (socklen_t *)&sin_size)) < 0)
    {
        printf("accept connection failed...\r\n");
        return -1;
    }
    printf("# Accept client %s\r\n", inet_ntoa(remote_addr.sin_addr));

    // ???Client?????????
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
     * ???????????????Server???Code end
     *
     * -----------------------------*/

    /* ------------------------------
     *
     * ??????????????????Server???Code begin
     *
     * -----------------------------*/
    double srvSum = 0.0f;
    double cltSum = 0.0f;
    double srvMax = 0.0f;
    double cltMax = 0.0f;
    // ------------------------ SUM begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadSum, &id[i]);

    // ???????????????????????????
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // ????????????????????????
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // ??????
    for (int i = 0; i < MAX_THREADS; i++) srvSum += doubleResults[i];
    printf("Srv max = %f\r\n", srvSum);

    if ((recv_len = recv(client_fd, &cltSum, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    finalSum = srvSum + cltSum;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-thread[SUM](Server): answer = %f, time = %ld us\r\n", finalSum, t_usec);
    // ------------------------ SUM end --------------------------

    // ------------------------ MAX begin ------------------------
    thread_begin = false;
    for (int i = 0; i < MAX_THREADS; i++) pthread_create(&tid[i], &attr, fnThreadMax, &id[i]);

    // ???????????????????????????
    gettimeofday(&startv, &startz);
    thread_begin = true;

    // ????????????????????????
    for (int i = 0; i < MAX_THREADS; i++) pthread_join(tid[i], NULL);

    // ??????
    srvMax = NewMax2(doubleResults, MAX_THREADS);

    if ((recv_len = recv(client_fd, &cltMax, sizeof(cltSum), 0)) < 0)
    {
        printf("receive client result failed...\r\n");
        return -1;
    }
    printf("Srv max = %f\r\n", srvMax);
    finalMax = srvMax > cltMax ? srvMax : cltMax;
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Multi-PC-multi-thread[MAX](Server): answer = %f, time = %ld us\r\n", finalMax, t_usec);
    // ------------------------ MAX end --------------------------

    // ----------------------- SORT begin ------------------------
    /* -----------------------------------------------------------
     * ?????????TCP?????????????????????????????????65536????????????8192????????????
     *
     * ????????????????????????????????????????????????1500????????????187????????????
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

    //     printf("%f\r\n", S_CLT_sortDoubleData[i]);
    // }

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

    merge2();
    // for (int i = 0; i < 16450; i++)
    // {
    //     printf("%f\r\n", S_sortDoubleData[i]);
    // }
    gettimeofday(&endv, &endz);
    t_usec = (endv.tv_sec - startv.tv_sec)*1000000 + (endv.tv_usec - startv.tv_usec);
    checkRlt = check(S_sortDoubleData, S_DATANUM);
    printf("Multi-PC-multi-thread[SORT](Server): answer = %d, time = %ld us\r\n", checkRlt, t_usec);
    // ------------------------ SORT end -------------------------

    /* ------------------------------
     *
     * ??????????????????Server???Code end
     *
     * -----------------------------*/

    close(client_fd);
    close(server_fd);
    return 0;
}
