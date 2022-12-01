#include "MyMatrix.hpp"

int main(int argc, char const *argv[])
{
     // 随机数种子
     srand(1);

     // 1. 无参构造
     MyMatrix<int> mat1;
     cout << "mat1 = " << endl
          << mat1 << endl;

     // 2. 方阵构造
     MyMatrix<float> mat2(3);
     cout << "mat2 = " << endl
          << mat2 << endl;

     // 3. 一般矩阵构造
     MyMatrix<double> mat3(2, 3);
     cout << "mat3 = " << endl
          << mat3 << endl;

     // 4. 把buffer转换成m*n的矩阵
     // [1, 2, 3,
     //  7, 8, 9]
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

     // 5. 复制构造
     MyMatrix<double> mat5(mat4);
     cout << "mat5 = " << endl
          << mat5 << endl;

     // 6. 加法
     cout << "mat4 + mat5 = " << endl
          << mat4 + mat5 << endl;
     mat4 += mat5;
     cout << "after +=, mat4 = " << endl
          << mat4 << endl;

     // 7. 减法
     cout << "mat4 - mat5 = " << endl
          << mat4 - mat5 << endl;
     mat4 -= mat5;
     cout << "after -=, mat4 = " << endl
          << mat4 << endl;

     // 8. 数乘
     cout << "mat4 * 2.0 = " << endl
          << mat4 * 2.0 << endl;
     mat4 *= 2.0;
     cout << "after *=, mat4 = " << endl
          << mat4 << endl;

     // 9. 乘法
     cout << "mat4 * mat5.T = " << endl
          << mat4 * mat5.transpose() << endl;
     mat4 *= mat5.transpose();
     cout << "after *=, mat4 = " << endl
          << mat4 << endl;

     // 10. 获取成员
     cout << "mat4.row = " << mat4.getRow() << endl;

     // 11. 修改成员
     mat4.setData(0, 1, 3.14);
     cout << "set mat4(0, 1) = 3.14, mat4 = " << endl
          << mat4 << endl;

     // 12. 乘法加速
     int size = 500;
     MyMatrix<int> a = MyMatrix<int>::random(size, size, 10);
     MyMatrix<int> b = MyMatrix<int>::random(size, size, 10);
     MyMatrix<int> c;

     // 计时相关
     struct timeval startv, endv;
     struct timezone startz, endz;
     // 时间间隔，单位us
     long t_usec_base = 0;
     long t_usec_improved = 0;

     gettimeofday(&startv, &startz);
     c = a.multiply(b);
     // cout << c << endl;
     gettimeofday(&endv, &endz);
     t_usec_base = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
     printf("Method ijk: duration = %ld us\n", t_usec_base);

     gettimeofday(&startv, &startz);
     c = a.multiply_jki(b);
     // cout << c << endl;
     gettimeofday(&endv, &endz);
     t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
     printf("Method jki: duration = %ld us\n", t_usec_improved);
     cout << "acc = " << (double)t_usec_base / t_usec_improved << endl;

     gettimeofday(&startv, &startz);
     c = a.multiply_ikj(b);
     // cout << c << endl;
     gettimeofday(&endv, &endz);
     t_usec_improved = (endv.tv_sec - startv.tv_sec) * 1000000 + (endv.tv_usec - startv.tv_usec);
     printf("Method ikj: duration = %ld us\n", t_usec_improved);
     cout << "acc = " << (double)t_usec_base / t_usec_improved << endl;

     return 0;
}
