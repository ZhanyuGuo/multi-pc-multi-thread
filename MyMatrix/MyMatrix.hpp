/* ----------------------------------------------------------
    作业1、二维矩阵库编写

    要求：
        a、支持不同大小的矩阵，并可以放入不同的数据类型
        b、构造方法：(1) 默认构造；(2) 方阵构造；(3) 一般矩阵构造；(4)复制构造；
        c、运算符重载：(1) 加法 + ；(2)减法 - ；(3) 矩阵数乘、矩阵相乘 * ；(4) 复制赋值 ；(5) 输出流运算符 <<
        d、其他方法：(1) 获取成员；(2) 修改成员；(3) 动态内存分配
        评分标准，功能实现60分；实现矩阵求逆和伪逆且考虑异常情况的，矩阵乘法写普通版版本和加速版本的，且加速比大于2 .0的10分；在python环境中调用自己写的程序，并和python直接实现以及调用numpy实现矩阵乘法三者进行比较和分析30分（不能直接使用时，可以借助于cython最终实现最终在python的调用）；提交源代码和工程文件（每人一份），提交的压缩包大于5MB的扣5分。

    矩阵类名 : MyMatrix，支持一下使用方法
    MyMatrix<int> temp;
    MyMatrix<double> temp(m, n);      //构造m*n的空矩阵
    MyMatrix<int> temp(buffer, m, n); //把buffer转换成m*n矩阵

    MyMatrix<int> temp2(temp);
    MyMatrix<int> temp3;
    temp3 = temp; //所有元素都要拷贝过去，深拷贝

    temp = temp + temp2; //同样大小的矩阵
    temp2 = temp * 3;    //每个元素乘以3
    temp2 = temp - 3;    //每个元素加3
    temp *= temp3;       //支持矩阵乘法和矩阵向量乘，如果不能乘返回值

    cout << temp << endl; //输出temp格式到屏幕
---------------------------------------------------------- */
#include <iostream>
using namespace std;

template <typename T>
class MyMatrix
{
private:
    T **m_data;   // 二维数据缓存
    int m_row;    // 行数
    int m_column; // 列数
    T temp;       // 临时变量

    void initMatrix();
    void deleteMatrix();

public:
    // 构造函数
    MyMatrix();                                             // 无参构造
    MyMatrix(int size);                                     // 方阵构造
    MyMatrix(int row, int column);                          // 一般矩阵构造
    MyMatrix(T **buffer, int row, int column);              // 把buffer转换成m*n的矩阵
    MyMatrix(const MyMatrix &otherMatrix);                  // 复制构造
    static MyMatrix random(int row, int column, int upper); // 随机数矩阵

    // 析构函数
    ~MyMatrix();

    // 获取成员
    int getRow() const;
    int getColumn() const;
    T getData(int i, int j) const;

    // 修改成员
    void setData(int i, int j, T data);

    // 运算
    MyMatrix add(const MyMatrix &otherMatrix) const;
    MyMatrix subtract(const MyMatrix &otherMatrix) const;
    MyMatrix multiply(const double num) const;
    MyMatrix multiply(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_jki(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_ikj(const MyMatrix &otherMatrix) const;
    MyMatrix transpose();

    // 重载运算符
    MyMatrix operator+(const MyMatrix &otherMatrix) const;
    MyMatrix operator+=(const MyMatrix &otherMatrix);
    MyMatrix operator-(const MyMatrix &otherMatrix) const;
    MyMatrix operator-=(const MyMatrix &otherMatrix);
    MyMatrix operator*(const double num) const;
    MyMatrix operator*=(const double num);
    MyMatrix operator*(const MyMatrix &otherMatrix) const;
    MyMatrix operator*=(const MyMatrix &otherMatrix);
    MyMatrix &operator=(const MyMatrix &otherMatrix); // 动态内存分配
    T &operator()(int i, int j);

    friend ostream &operator<<(ostream &out, const MyMatrix<T> &obj)
    {
        out << "[";
        for (int i = 0; i < obj.m_row; i++)
        {
            if (i != 0)
                out << " ";
            for (int j = 0; j < obj.m_column; j++)
            {
                out << obj.m_data[i][j];
                if (i != obj.m_row - 1 || j != obj.m_column - 1)
                    out << ", ";
            }
            if (i != obj.m_row - 1)
                out << endl;
        }
        out << "]" << endl;

        return out;
    }
};

template <typename T>
void MyMatrix<T>::initMatrix()
{
    m_data = new T *[m_row];
    for (int i = 0; i < m_row; i++)
        m_data[i] = new T[m_column];

    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            m_data[i][j] = 0;
}

template <typename T>
void MyMatrix<T>::deleteMatrix()
{
    for (int i = 0; i < m_row; i++)
        delete[] m_data[i];
    delete[] m_data;
}

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
MyMatrix<T>::MyMatrix(T **buffer, int row, int column) : m_row(row), m_column(column)
{
    initMatrix();
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            m_data[i][j] = buffer[i][j];
}

template <typename T>
MyMatrix<T>::MyMatrix(const MyMatrix &otherMatrix)
{
    m_row = otherMatrix.m_row;
    m_column = otherMatrix.m_column;
    initMatrix();

    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            m_data[i][j] = otherMatrix.m_data[i][j];
}

template <typename T>
MyMatrix<T> MyMatrix<T>::random(int row, int column, int upper)
{
    MyMatrix<T> result(row, column);
    for (int i = 0; i < row; i++)
        for (int j = 0; j < column; j++)
            result.m_data[i][j] = rand() % upper;

    return result;
}

template <typename T>
MyMatrix<T>::~MyMatrix()
{
    deleteMatrix();
}

template <typename T>
int MyMatrix<T>::getRow() const
{
    return m_row;
}

template <typename T>
int MyMatrix<T>::getColumn() const
{
    return m_column;
}

template <typename T>
T MyMatrix<T>::getData(int i, int j) const
{
    if (i < 0 || i >= m_row || j < 0 || j >= m_column)
    {
        cout << "INDEX ERROR!" << endl;
        return temp;
    }

    return m_data[i][j];
}

template <typename T>
void MyMatrix<T>::setData(int i, int j, T data)
{
    if (i < 0 || i >= m_row || j < 0 || j >= m_column)
    {
        cout << "INDEX ERROR!" << endl;
        return;
    }

    m_data[i][j] = data;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::add(const MyMatrix &otherMatrix) const
{
    if (otherMatrix.m_row != m_row || otherMatrix.m_column != m_column)
    {
        cout << "SIZE ERROR!" << endl;
        return *this;
    }

    MyMatrix result(m_row, m_column);
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            result.m_data[i][j] = m_data[i][j] + otherMatrix.m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::subtract(const MyMatrix &otherMatrix) const
{
    if (otherMatrix.m_row != m_row || otherMatrix.m_column != m_column)
    {
        cout << "SIZE ERROR!" << endl;
        return *this;
    }

    MyMatrix result(m_row, m_column);
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            result.m_data[i][j] = m_data[i][j] - otherMatrix.m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply(const double num) const
{
    MyMatrix result(m_row, m_column);
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            result.m_data[i][j] = num * m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply(const MyMatrix &otherMatrix) const
{
    if (m_column != otherMatrix.m_row)
    {
        cout << "SIZE ERROR!" << endl;
        return *this;
    }

    MyMatrix result(m_row, otherMatrix.m_column);
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < otherMatrix.m_column; j++)
            for (int k = 0; k < m_column; k++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_jki(const MyMatrix &otherMatrix) const
{
    MyMatrix result(m_row, otherMatrix.m_column);
    if (m_column != otherMatrix.m_row)
    {
        cout << "SIZE ERROR!" << endl;
        return *this;
    }

    for (int j = 0; j < otherMatrix.m_column; j++)
        for (int k = 0; k < m_column; k++)
            for (int i = 0; i < m_row; i++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_ikj(const MyMatrix &otherMatrix) const
{
    MyMatrix result(m_row, otherMatrix.m_column);
    if (m_column != otherMatrix.m_row)
    {
        cout << "SIZE ERROR!" << endl;
        return *this;
    }

    for (int i = 0; i < m_row; i++)
        for (int k = 0; k < m_column; k++)
            for (int j = 0; j < otherMatrix.m_column; j++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::transpose()
{
    MyMatrix<T> result(m_column, m_row);
    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            result.m_data[j][i] = m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator+(const MyMatrix &otherMatrix) const
{
    return add(otherMatrix);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator+=(const MyMatrix &otherMatrix)
{
    *this = add(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator-(const MyMatrix &otherMatrix) const
{
    return subtract(otherMatrix);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator-=(const MyMatrix &otherMatrix)
{
    *this = subtract(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator*(const double num) const
{
    return multiply(num);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator*=(const double num)
{
    *this = multiply(num);
    return *this;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator*(const MyMatrix &otherMatrix) const
{
    return multiply(otherMatrix);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator*=(const MyMatrix &otherMatrix)
{
    *this = multiply(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator=(const MyMatrix &otherMatrix)
{
    if (m_row != otherMatrix.m_row || m_column != otherMatrix.m_column)
    {
        deleteMatrix();
        m_row = otherMatrix.m_row;
        m_column = otherMatrix.m_column;
        initMatrix();
    }

    for (int i = 0; i < m_row; i++)
        for (int j = 0; j < m_column; j++)
            m_data[i][j] = otherMatrix.m_data[i][j];

    return *this;
}

template <typename T>
T &MyMatrix<T>::operator()(int i, int j)
{
    if (i < 0 || i >= m_row || j < 0 || j >= m_column)
    {
        cout << "INDEX ERROR!" << endl;
        return temp;
    }

    return m_data[i][j];
}
