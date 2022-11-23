#include <iostream>
using namespace std;

// a、支持不同大小的矩阵，并可以放入不同的数据类型
// b、构造方法：(1)默认构造；(2)方阵构造；(3)一般矩阵构造； (4)复制构造；
// c、运算符重载：(1)加法 + ；(2)减法 - ；(3)矩阵数乘、矩阵相乘 * ；(4)复制赋值 ；(5)输出流运算符 <<
// d、其他方法：(1)获取成员；(2)修改成员；(3)动态内存分配

template <typename T>
class MyMatrix
{
private:
    T **m_data;
    int m_row;
    int m_column;

    void initMatrix();
    void deleteMatrix();

public:
    MyMatrix();
    MyMatrix(int size);
    MyMatrix(int row, int column);
    MyMatrix(int row, int column, T array);

    ~MyMatrix();
};

template <typename T>
MyMatrix<T>::MyMatrix() : m_row(0), m_column(0)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(int size) : m_row(size), m_column(size)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(int row, int column) : m_row(row), m_column(column)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(int row, int column, T array) : m_row(row), m_column(column)
{
    initMatrix();

    // NOTE: HERE
}

template <typename T>
MyMatrix<T>::~MyMatrix()
{
    deleteMatrix();
}

template <typename T>
void MyMatrix<T>::initMatrix()
{
    m_data = new T *[m_row];
    for (size_t i = 0; i < m_row; i++)
    {
        m_data[i] = new T[m_column];
    }
}

template <typename T>
void MyMatrix<T>::deleteMatrix()
{
    for (size_t i = 0; i < m_row; i++)
    {
        delete[] m_data[i];
    }
    delete[] m_data;
}