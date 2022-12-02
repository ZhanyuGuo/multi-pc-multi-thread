#include <sys/time.h>
#include <pthread.h>
#include "MyMatrix.hpp"

#define SIZE 2000
#define MAX_THREADS SIZE

MyMatrix<double> result(SIZE, SIZE);
MyMatrix<double> a = MyMatrix<double>::random(SIZE, SIZE, 10);
MyMatrix<double> b = MyMatrix<double>::random(SIZE, SIZE, 10);
MyMatrix<double> c;

void *fnThreadMultiply_row_divide(void *arg)
{
    int who = *(int *)arg; // 线程ID
    int i = who;

    for (int j = 0; j < SIZE; j++)
        for (int k = 0; k < SIZE; k++)
            result(i, j) += a(i, k) * b(k, j);

    return NULL;
}

int main(int argc, char const *argv[])
{
    srand(1);

    // 计时相关
    struct timeval startv, endv;
    struct timezone startz, endz;
    // 时间间隔，单位us
    long t_usec_base = 0;
    long t_usec_improved = 0;

    // ================================ method ijk ================================
    gettimeofday(&startv, &startz);
    c = a.multiply(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_base = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method ijk: duration = %ld us\n", t_usec_base);

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

    gettimeofday(&startv, &startz);
    for (int i = 0; i < MAX_THREADS; i++)
        pthread_create(&tid[i], &attr, fnThreadMultiply_row_divide, &id[i]);

    for (int i = 0; i < MAX_THREADS; i++)
        pthread_join(tid[i], NULL);

    gettimeofday(&endv, &endz);
    t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    // cout << result << endl;
    printf("Method threads: duration = %ld us\n", t_usec_improved);
    cout << "threads acc = " << (double)t_usec_base / t_usec_improved << endl;

    return 0;
}
