#include <sys/time.h>
#include "MyMatrix.hpp"

/*
    Use the following command to generate .so file for the use of MyMatrix.py

    ```bash
        g++ -o multiply_test.so -shared -fPIC cpp2python.cpp
    ```
*/

extern "C"
{
    double multiply(int size, int mode, bool screen)
    {
        // 计时相关
        struct timeval startv, endv;
        struct timezone startz, endz;
        // 时间间隔，单位us
        long t_usec = 0;

        cout << "Here is cpp!" << endl;
        MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
        MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);
        MyMatrix<int> c;

        gettimeofday(&startv, &startz);
        switch (mode)
        {
        case 0:
            c = a.multiply(b);
            break;
        case 1:
            c = a.multiply_ikj(b);
            break;
        case 2:
            c = a.multiply_jki(b);
            break;
        case 3:
            c = a.multiply_ikj_acc(b);
            break;
        default:
            break;
        }
        if (screen)
            cout << c << endl;
        gettimeofday(&endv, &endz);
        t_usec = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);

        return t_usec / 1000000.0;
    }
}
