#include <sys/time.h>
#include "MyMatrix.hpp"

int main(int argc, char const *argv[])
{
    // 随机数种子
    srand(1);

    // 1. 无参构造
    MyMatrix<int> mat1;
    cout << "无参构造 mat1 = \n"
         << mat1 << endl;

    // 2. 方阵构造
    MyMatrix<float> mat2(3);
    cout << "方阵构造 mat2 = \n"
         << mat2 << endl;

    // 3. 一般矩阵构造
    MyMatrix<double> mat3(2, 3);
    cout << "一般矩阵构造 mat3 = \n"
         << mat3 << endl;

    // 4. 把buffer转换成m*n的矩阵
    // [1, 2, 3;
    //  7, 8, 9]
    int **buffer;
    buffer = new int *[2];
    buffer[0] = new int[3];
    buffer[1] = new int[3];
    buffer[0][0] = 1;
    buffer[0][1] = 2;
    buffer[0][2] = 3;
    buffer[1][0] = 7;
    buffer[1][1] = 8;
    buffer[1][2] = 9;

    MyMatrix<int> mat4(buffer, 2, 3);
    cout << "buff构造 mat4 = \n"
         << mat4 << endl;

    delete[] buffer[0];
    delete[] buffer[1];
    delete[] buffer;

    // 5. 复制构造
    MyMatrix<int> mat5(mat4);
    cout << "复制构造 mat5 := mat4 = \n"
         << mat5 << endl;

    // 6. 加减乘法（数）
    cout << "mat4 + 66 = \n"
         << mat4 + 66 << endl;
    mat4 += 66;
    cout << "mat4 += 66, mat4 = \n"
         << mat4 << endl;

    cout << "mat4 - 66 = \n"
         << mat4 - 66 << endl;
    mat4 -= 66;
    cout << "mat4 -= 66, mat4 = \n"
         << mat4 << endl;

    cout << "mat4 * 66 = \n"
         << mat4 * 66 << endl;
    mat4 *= 66;
    cout << "mat4 *= 66, mat4 = \n"
         << mat4 << endl;

    // 7. 加减乘法（矩阵）
    cout << "mat4 = mat5 = \n"
         << (mat4 = mat5) << endl;
    cout << "mat4 + mat5 = \n"
         << mat4 + mat5 << endl;
    mat4 += mat5;
    cout << "mat4 += mat5, mat4 = \n"
         << mat4 << endl;

    cout << "mat4 - mat5 = \n"
         << mat4 - mat5 << endl;
    mat4 -= mat5;
    cout << "mat4 -= mat5, mat4 = \n"
         << mat4 << endl;

    cout << "mat4 * mat5.T = \n"
         << mat4 * mat5.transpose() << endl;
    mat4 *= mat5.transpose();
    cout << "mat4 *= mat5.T, mat4 = \n"
         << mat4 << endl;

    // 8. 获取成员
    cout << "mat4.row = " << mat4.getRows() << ", mat4.col = " << mat4.getCols() << endl;
    cout << "get mat4(0, 1) = " << mat4(0, 1) << endl;

    // 9. 修改成员
    mat4(0, 1) = 666;
    cout << "set mat4(0, 1) = 666, mat4 = \n"
         << mat4 << endl;

    // 10. 求逆
    MyMatrix<double> mat6(3, 3);
    mat6(0, 2) = 1.0;
    mat6(1, 0) = 1.0;
    mat6(2, 1) = 1.0;
    cout << "mat6 = \n"
         << mat6 << endl;
    cout << "mat6.inverse = \n"
         << mat6.inverse() << endl;
    cout << "mat6 * mat6.inverse = \n"
         << mat6 * mat6.inverse() << endl;

    mat6 = MyMatrix<double>::random(3, 3);
    cout << "mat6 = \n"
         << mat6 << endl;
    cout << "mat6.inverse = \n"
         << mat6.inverse() << endl;
    cout << "mat6 * mat6.inverse = \n"
         << mat6 * mat6.inverse() << endl;

    // 11. 乘法加速
    int size = 2000; // NOTE: set 2000 to test
    MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
    MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);
    MyMatrix<int> c;

    // 计时相关
    struct timeval startv, endv;
    struct timezone startz, endz;
    // 时间间隔，单位us
    long t_usec_base = 0;
    long t_usec_improved = 0;

    cout << "Calculating...(about 60s on my machine, size=2000)" << endl;

    // normal
    gettimeofday(&startv, &startz);
    c = a.multiply(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_base = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method ijk: duration = %ld us\n", t_usec_base);

    // normal acc, analog parallelism
    gettimeofday(&startv, &startz);
    c = a.multiply_acc(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method ijk acc: duration = %ld us, acc = %lf\n", t_usec_improved, (double)t_usec_base / t_usec_improved);

    // jki
    gettimeofday(&startv, &startz);
    c = a.multiply_jki(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method jki: duration = %ld us, acc = %lf\n", t_usec_improved, (double)t_usec_base / t_usec_improved);

    // ikj
    gettimeofday(&startv, &startz);
    c = a.multiply_ikj(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method ikj: duration = %ld us, acc = %lf\n", t_usec_improved, (double)t_usec_base / t_usec_improved);

    // ikj acc
    gettimeofday(&startv, &startz);
    c = a.multiply_ikj_acc(b);
    // cout << c << endl;
    gettimeofday(&endv, &endz);
    t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
    printf("Method ikj acc: duration = %ld us, acc = %lf\n", t_usec_improved, (double)t_usec_base / t_usec_improved);

    return 0;
}
