#include "MyMatrix.hpp"
#include <ctime>

int main(int argc, char const *argv[])
{
    srand(1);

    // 无参数构造
    MyMatrix<int> mat1;
    cout << "mat1 = " << endl
         << mat1 << endl;

    // 方阵构造
    MyMatrix<float> mat2(3);
    cout << "mat2 = " << endl
         << mat2 << endl;

    // 一般矩阵构造
    MyMatrix<double> mat3(2, 3);
    cout << "mat3 = " << endl
         << mat3 << endl;

    // 把buffer转换成m*n矩阵
    double **buffer;
    buffer = new double *[2];
    buffer[0] = new double[3];
    buffer[1] = new double[3];
    buffer[0][0] = 1;
    buffer[0][1] = 2;
    buffer[0][2] = 3;
    buffer[1][0] = 7;
    buffer[1][1] = 8;
    buffer[1][2] = 9;

    MyMatrix<double> mat4(buffer, 2, 3);
    cout << "mat4 = " << endl
         << mat4 << endl;

    delete[] buffer[0];
    delete[] buffer[1];
    delete[] buffer;

    // 复制构造
    MyMatrix<double> mat5(mat4);
    cout << "mat5 = " << endl
         << mat5 << endl;

    // 加法
    cout << "mat4 + mat5 = " << endl
         << mat4 + mat5 << endl;
    mat4 += mat5;
    cout << "after +=, mat4 = " << endl
         << mat4 << endl;

    // 减法
    cout << "mat4 - mat5 = " << endl
         << mat4 - mat5 << endl;
    mat4 -= mat5;
    cout << "after -=, mat4 = " << endl
         << mat4 << endl;

    // 乘法
    cout << "mat4 * 2.0 = " << endl
         << mat4 * 2.0 << endl;
    mat4 *= 2.0;
    cout << "after *=, mat4 = " << endl
         << mat4 << endl;

    cout << "mat4 * mat5.T = " << endl
         << mat4 * mat5.transpose() << endl;
    mat4 *= mat5.transpose();
    cout << "after *=, mat4 = " << endl
         << mat4 << endl;

    // 获取成员
    cout << "mat4.row = " << mat4.getRow() << endl;

    // 修改成员
    mat4.setData(0, 1, 3.14);
    cout << "set mat4(0, 1) = 3.14, mat4 = " << endl
         << mat4 << endl;

    // 乘法加速: why? Maybe set_data is more time consuming...
    int size = 100;
    MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
    MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);

    clock_t start, end;
    double time1, time2;

    start = clock();
    // cout << a.multiply(b) << endl;
    a.multiply(b);
    end = clock();
    time1 = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Method ijk: duration = %.5lfs\n", time1);

    start = clock();
    // cout << a.multiply_ikj(b) << endl;
    a.multiply_jki(b);
    end = clock();
    time2 = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Method jki: duration = %.5lfs\n", time2);

    cout << "acc = " << time1 / time2 << endl;

    return 0;
}
