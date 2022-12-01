# 矩阵库
作者: 郭展羽, 学号: 2230730

## MyMatrix.cpp & MyMatrix.hpp
- 完成除矩阵求逆和伪逆外所有的要求.
- 矩阵乘法加速: 使用交换遍历次序达到了最高4倍多的加速效果.

## MyMatrix.py
- 使用list实现矩阵乘法.
- 使用Numpy进行矩阵乘法.
- 调用CPP进行矩阵乘法.


## cpp2python.cpp
- 生成`.so`文件供`MyMatrix.py`程序测试.

## multiply_test.so
- `cpp2python.cpp`生成的文件.

## multiply_threads.cpp
- 使用多线程进行矩阵乘法的加速, 最高加速比未知(Size=500时达到了5.4倍).
