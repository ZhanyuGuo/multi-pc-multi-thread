# 矩阵库
作者: 郭展羽, 学号: 2230730

## MyMatrix.cpp & MyMatrix.hpp
- 完成除矩阵求伪逆外所有的要求.
- 矩阵乘法加速: 使用交换遍历次序加速, 最高加速比未知(Size=2000时达到了4.39倍).
```bash
Calculating...(about 60s on my machine, size=2000)
Method ijk: duration = 64143253 us
Method ijk acc: duration = 24398109 us, acc = 2.629026
Method jki: duration = 82430102 us, acc = 0.778153
Method ikj: duration = 29826358 us, acc = 2.150556
Method ikj acc: duration = 14621590 us, acc = 4.386886
```

## multiply_threads.cpp
- 使用多线程进行矩阵乘法的加速, 最高加速比未知(Size=2000时达到了4.52倍).
```bash
Calculating...(about 80s on my machine)
Method ijk: duration = 82886634 us
Method threads: duration = 18344833 us, acc = 4.518255
```

## MyMatrix.py
- SIZE = 500(因为用list已经需要20s)
- 使用list实现矩阵乘法. (基准)
- 使用Numpy进行矩阵乘法. (加速193.11倍)
- 调用CPP进行矩阵乘法. (加速29.97倍)
- 调用CPP进行矩阵加速乘法. (加速75.24倍)
```bash
Calculating...(about 20s on my machine)
python normal: duration = 17.3643 s
numpy: duration = 0.0911 s, acc = 190.57
Here is cpp, mode = 0
cpp ijk: duration = 0.5876 s, acc = 29.55
Here is cpp, mode = 3
cpp ikj acc: duration = 0.2323 s, acc = 74.76
```

## cpp2python.cpp
- 生成`.so`文件供`MyMatrix.py`程序测试.

## multiply_test.so
- `cpp2python.cpp`生成的文件.
