#include "mySrvClt.hpp"

/*
    Use the following command to generate .so file for the use of MyMax.py

    ```bash
        g++ -o max_test.so -shared -fPIC cpp2python.cpp
    ```
*/

extern "C"
{
    double *initData()
    {
        for (int i = 0; i < DATANUM; i++)
            rawDoubleData[i] = fabs(rand());

        return rawDoubleData;
    }

    double maxNormal()
    {
        return max(rawDoubleData, DATANUM);
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

    double maxAcc()
    {
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

        thread_begin = false;
        for (int i = 0; i < MAX_THREADS; i++)
            pthread_create(&tid[i], &attr, fnThreadMax, &id[i]);

        // 给多个线程同时发令
        thread_begin = true;

        // 等待线程运行结束
        for (int i = 0; i < MAX_THREADS; i++)
            pthread_join(tid[i], NULL);

        // 收割
        finalMax = maxMerge(doubleResults, MAX_THREADS);

        return finalMax;
    }
}