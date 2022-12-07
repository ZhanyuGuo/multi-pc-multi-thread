#include "MyMatrix.hpp"

/*
    Use the following command to generate .so file for the use of MyMatrix.py

    ```bash
        g++ -o multiply_test.so -shared -fPIC cpp2python.cpp
    ```
*/

extern "C"
{
    void multiply(int size, int mode, bool screen)
    {

        printf("Here is cpp, mode = %d\n", mode);

        MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
        MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);
        MyMatrix<int> c;

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

        return;
    }
}
