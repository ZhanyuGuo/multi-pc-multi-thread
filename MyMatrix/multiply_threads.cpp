// 多线程无法加速，但能算到正确的结果
#include <iostream>
#include <pthread.h>
#include <ctime>
#include "MyMatrix.hpp"
using namespace std;

#define SIZE 1000
// #define MAX_THREADS SIZE *SIZE
#define MAX_THREADS SIZE

MyMatrix<double> result(SIZE, SIZE);
MyMatrix<double> a = MyMatrix<double>::random(SIZE, SIZE, 10);
MyMatrix<double> b = MyMatrix<double>::random(SIZE, SIZE, 10);

// void *fnThreadMultiply(void *arg)
// {
//     int who = *(int *)arg; // 线程ID
//     int i = who / SIZE;
//     int j = who % SIZE;
//     int sum = 0;
//     for (size_t k = 0; k < SIZE; k++)
//     {
//         sum += a.getData(i, k) * b.getData(k, j);
//     }
//     result.setData(i, j, sum);

//     return NULL;
// }

void *fnThreadMultiply(void *arg)
{
    int who = *(int *)arg; // 线程ID
    int i = who % SIZE;

    for (size_t j = 0; j < SIZE; j++)
    {
        int sum = 0;
        for (size_t k = 0; k < SIZE; k++)
        {
            sum += a.getData(i, k) * b.getData(k, j);
        }
        result.setData(i, j, sum);
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    srand(1);

    clock_t start, end;
    double base_time, improve_time;

    // ================================ method ijk ================================
    start = clock();
    // cout << a.multiply(b) << endl;
    a.multiply(b);
    end = clock();
    base_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Method ijk: duration = %.5lfs\n", base_time);

    // ================================ method jki ================================
    start = clock();
    // cout << a.multiply_jki(b) << endl;
    a.multiply_jki(b);
    end = clock();
    improve_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Method jki: duration = %.5lfs\n", improve_time);

    cout << "jki acc = " << base_time / improve_time << endl;

    // ================================ method threads ================================
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

    start = clock();
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadMultiply, &id[i]);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    end = clock();
    improve_time = (double)(end - start) / CLOCKS_PER_SEC;
    // cout << result << endl;
    printf("Method threads: duration = %.5lfs\n", improve_time);
    cout << "threads acc = " << base_time / improve_time << endl;

    return 0;
}
