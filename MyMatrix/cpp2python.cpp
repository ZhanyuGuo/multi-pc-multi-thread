#include <ctime>
#include "MyMatrix.hpp"

/*

    g++ -o multiplt_test.so -shared -fPIC cpp2python.cpp

*/

extern "C"
{
    double multiply(int size, int mode, bool screen)
    {
        clock_t start, end;

        cout << "Here is cpp!" << endl;
        MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
        MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);

        start = clock();
        MyMatrix<int> result;
        switch (mode)
        {
        case 0:
            result = a.multiply(b);
            break;
        case 1:
            result = a.multiply_ikj(b);
            break;
        case 2:
            result = a.multiply_jki(b);
            break;
        default:
            break;
        }
        if (screen)
            cout << result << endl;
        end = clock();

        double time = (double)(end - start) / CLOCKS_PER_SEC;
        // cout << time << endl;

        return time;
    }
}