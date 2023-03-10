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
#include <assert.h>
using namespace std;

template <typename T>
class MyMatrix
{
private:
    T **m_data; // 二维数据缓存
    int m_rows; // 行数
    int m_cols; // 列数

    void initMatrix();
    void deleteMatrix();

    void rowTrans(int src, T k, int dst = -1);
    void rowSwap(int src, int dst);

public:
    // 构造函数
    MyMatrix();                                                 // 无参构造
    MyMatrix(int size);                                         // 方阵构造
    MyMatrix(int rows, int cols);                               // 一般矩阵构造
    MyMatrix(T **buff, int rows, int cols);                     // 把buffer转换成m*n的矩阵
    MyMatrix(const MyMatrix &otherMatrix);                      // 复制构造
    static MyMatrix random(int rows, int cols, int upper = 10); // 随机数矩阵

    // 析构函数
    ~MyMatrix();

    // 获取成员
    int getRows() const;
    int getCols() const;
    T getData(int i, int j) const;

    // 修改成员
    void setData(int i, int j, T data);

    // 运算
    MyMatrix add(const T &num) const;
    MyMatrix add(const MyMatrix &otherMatrix) const;
    MyMatrix subtract(const T &num) const;
    MyMatrix subtract(const MyMatrix &otherMatrix) const;
    MyMatrix multiply(const T &num) const;
    MyMatrix multiply(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_acc(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_jki(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_ikj(const MyMatrix &otherMatrix) const;
    MyMatrix multiply_ikj_acc(const MyMatrix &otherMatrix) const;
    MyMatrix transpose() const;
    MyMatrix inverse() const;

    // 重载运算符
    MyMatrix operator+(const T &num) const;
    MyMatrix operator+(const MyMatrix &otherMatrix) const;
    MyMatrix operator-(const T &num) const;
    MyMatrix operator-(const MyMatrix &otherMatrix) const;
    MyMatrix operator*(const T &num) const;
    MyMatrix operator*(const MyMatrix &otherMatrix) const;

    MyMatrix &operator+=(const T &num);
    MyMatrix &operator+=(const MyMatrix &otherMatrix);
    MyMatrix &operator-=(const T &num);
    MyMatrix &operator-=(const MyMatrix &otherMatrix);
    MyMatrix &operator*=(const T &num);
    MyMatrix &operator*=(const MyMatrix &otherMatrix);
    MyMatrix &operator=(const MyMatrix &otherMatrix); // 动态内存分配
    T &operator()(int i, int j);

    friend ostream &operator<<(ostream &out, const MyMatrix<T> &obj)
    {
        out << "[";
        for (int i = 0; i < obj.m_rows; i++)
        {
            if (i != 0)
                out << " ";
            for (int j = 0; j < obj.m_cols; j++)
            {
                out << obj.m_data[i][j];
                if (i != obj.m_rows - 1 || j != obj.m_cols - 1)
                    out << ", ";
            }
            if (i != obj.m_rows - 1)
                out << endl;
        }
        out << "]" << endl;

        return out;
    }
};

template <typename T>
void MyMatrix<T>::initMatrix()
{
    m_data = new T *[m_rows];
    for (int i = 0; i < m_rows; i++)
        m_data[i] = new T[m_cols];

    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            m_data[i][j] = (T)0;
}

template <typename T>
void MyMatrix<T>::deleteMatrix()
{
    for (int i = 0; i < m_rows; i++)
        delete[] m_data[i];
    delete[] m_data;
}

template <typename T>
MyMatrix<T>::MyMatrix() : m_rows(0), m_cols(0)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(int size) : m_rows(size), m_cols(size)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(int rows, int cols) : m_rows(rows), m_cols(cols)
{
    initMatrix();
}

template <typename T>
MyMatrix<T>::MyMatrix(T **buff, int rows, int cols) : m_rows(rows), m_cols(cols)
{
    initMatrix();
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            m_data[i][j] = buff[i][j];
}

template <typename T>
MyMatrix<T>::MyMatrix(const MyMatrix &otherMatrix)
{
    m_rows = otherMatrix.m_rows;
    m_cols = otherMatrix.m_cols;
    initMatrix();

    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            m_data[i][j] = otherMatrix.m_data[i][j];
}

template <typename T>
MyMatrix<T> MyMatrix<T>::random(int rows, int cols, int upper)
{
    MyMatrix result(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.m_data[i][j] = rand() % upper;

    return result;
}

template <typename T>
MyMatrix<T>::~MyMatrix()
{
    deleteMatrix();
}

template <typename T>
int MyMatrix<T>::getRows() const
{
    return m_rows;
}

template <typename T>
int MyMatrix<T>::getCols() const
{
    return m_cols;
}

template <typename T>
T MyMatrix<T>::getData(int i, int j) const
{
    assert(i >= 0);
    assert(i < m_rows);
    assert(j >= 0);
    assert(j < m_cols);

    return m_data[i][j];
}

template <typename T>
void MyMatrix<T>::setData(int i, int j, T data)
{
    assert(i >= 0);
    assert(i < m_rows);
    assert(j >= 0);
    assert(j < m_cols);

    m_data[i][j] = data;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::add(const T &num) const
{
    MyMatrix result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[i][j] = m_data[i][j] + num;

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::add(const MyMatrix &otherMatrix) const
{
    assert(m_rows == otherMatrix.m_rows);
    assert(m_cols == otherMatrix.m_cols);

    MyMatrix result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[i][j] = m_data[i][j] + otherMatrix.m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::subtract(const T &num) const
{
    MyMatrix result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[i][j] = m_data[i][j] - num;

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::subtract(const MyMatrix &otherMatrix) const
{
    assert(m_rows == otherMatrix.m_rows);
    assert(m_cols == otherMatrix.m_cols);

    MyMatrix result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[i][j] = m_data[i][j] - otherMatrix.m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply(const T &num) const
{
    MyMatrix result(m_rows, m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[i][j] = m_data[i][j] * num;

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply(const MyMatrix &otherMatrix) const
{
    assert(m_cols == otherMatrix.m_rows);

    MyMatrix result(m_rows, otherMatrix.m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < otherMatrix.m_cols; j++)
            for (int k = 0; k < m_cols; k++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_acc(const MyMatrix &otherMatrix) const
{
    assert(m_cols == otherMatrix.m_rows);

    MyMatrix result(m_rows, otherMatrix.m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < otherMatrix.m_cols; j += 4)
        {
            T sum1, sum2, sum3, sum4;
            sum1 = sum2 = sum3 = sum4 = 0;
            for (int k = 0; k < m_cols; k++)
            {
                T s = m_data[i][k];
                sum1 += s * otherMatrix.m_data[k][j];
                sum2 += s * otherMatrix.m_data[k][j + 1];
                sum3 += s * otherMatrix.m_data[k][j + 2];
                sum4 += s * otherMatrix.m_data[k][j + 3];
            }
            result.m_data[i][j] = sum1;
            result.m_data[i][j + 1] = sum2;
            result.m_data[i][j + 2] = sum3;
            result.m_data[i][j + 3] = sum4;
        }
    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_jki(const MyMatrix &otherMatrix) const
{
    assert(m_cols == otherMatrix.m_rows);

    MyMatrix result(m_rows, otherMatrix.m_cols);
    for (int j = 0; j < otherMatrix.m_cols; j++)
        for (int k = 0; k < m_cols; k++)
            for (int i = 0; i < m_rows; i++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_ikj(const MyMatrix &otherMatrix) const
{
    assert(m_cols == otherMatrix.m_rows);

    MyMatrix result(m_rows, otherMatrix.m_cols);
    for (int i = 0; i < m_rows; i++)
        for (int k = 0; k < m_cols; k++)
            for (int j = 0; j < otherMatrix.m_cols; j++)
                result.m_data[i][j] += m_data[i][k] * otherMatrix.m_data[k][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::multiply_ikj_acc(const MyMatrix &otherMatrix) const
{
    assert(m_cols == otherMatrix.m_rows);

    MyMatrix result(m_rows, otherMatrix.m_cols);
    for (int i = 0; i < m_rows; i++)
    {
        T *temp = new T[otherMatrix.m_cols];
        for (int j = 0; j < otherMatrix.m_cols; j++)
            temp[j] = 0;

        for (int k = 0; k < m_cols; k++)
        {
            T s = m_data[i][k];

            for (int j = 0; j < otherMatrix.m_cols; j += 4)
            {
                temp[j] += s * otherMatrix.m_data[k][j];
                temp[j + 1] += s * otherMatrix.m_data[k][j + 1];
                temp[j + 2] += s * otherMatrix.m_data[k][j + 2];
                temp[j + 3] += s * otherMatrix.m_data[k][j + 3];
            }
        }

        for (int j = 0; j < otherMatrix.m_cols; j++)
            result.m_data[i][j] = temp[j];
        delete[] temp;
    }

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::transpose() const
{
    MyMatrix result(m_cols, m_rows);
    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            result.m_data[j][i] = m_data[i][j];

    return result;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::inverse() const
{
    assert(m_rows == m_cols);

    MyMatrix result(m_cols, m_rows);
    for (int i = 0; i < m_rows; i++)
        result(i, i) = (T)1;

    MyMatrix working(*this);
    for (int j = 0; j < m_cols; j++)
    {
        bool reversibility = false;
        for (int i = j; i < m_rows; i++)
        {
            if (abs(working(i, j)) > 1e-5)
            {
                reversibility = true;
                if (i != j)
                {
                    working.rowSwap(i, j);
                    result.rowSwap(i, j);
                }
                break;
            }
        }
        assert(reversibility);

        T diag = working(j, j);
        for (int i = j + 1; i < m_rows; i++)
        {
            T target = working(i, j);
            working.rowTrans(j, -target / diag, i);
            result.rowTrans(j, -target / diag, i);
        }
        working.rowTrans(j, 1 / diag);
        result.rowTrans(j, 1 / diag);
    }

    for (int j = 0; j < m_cols; j++)
    {
        for (int i = 0; i < j; i++)
        {
            T target = working(i, j);
            working.rowTrans(j, -target, i);
            result.rowTrans(j, -target, i);
        }
    }

    return result;
}

template <typename T>
void MyMatrix<T>::rowTrans(int src, T k, int dst)
{
    if (dst == -1)
        for (int j = 0; j < m_cols; j++)
            m_data[src][j] *= k;
    else
        for (int j = 0; j < m_cols; j++)
            m_data[dst][j] += k * m_data[src][j];
}

template <typename T>
void MyMatrix<T>::rowSwap(int src, int dst)
{
    T temp;

    for (int j = 0; j < m_cols; j++)
    {
        temp = m_data[src][j];
        m_data[src][j] = m_data[dst][j];
        m_data[dst][j] = temp;
    }
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator+(const T &num) const
{
    return add(num);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator+(const MyMatrix &otherMatrix) const
{
    return add(otherMatrix);
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator+=(const T &num)
{
    *this = add(num);
    return *this;
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator+=(const MyMatrix &otherMatrix)
{
    *this = add(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator-(const T &num) const
{
    return subtract(num);
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator-(const MyMatrix &otherMatrix) const
{
    return subtract(otherMatrix);
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator-=(const T &num)
{
    *this = subtract(num);
    return *this;
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator-=(const MyMatrix &otherMatrix)
{
    *this = subtract(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> MyMatrix<T>::operator*(const T &num) const
{
    return multiply(num);
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator*=(const T &num)
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
MyMatrix<T> &MyMatrix<T>::operator*=(const MyMatrix &otherMatrix)
{
    *this = multiply(otherMatrix);
    return *this;
}

template <typename T>
MyMatrix<T> &MyMatrix<T>::operator=(const MyMatrix &otherMatrix)
{
    if (m_rows != otherMatrix.m_rows || m_cols != otherMatrix.m_cols)
    {
        deleteMatrix();
        m_rows = otherMatrix.m_rows;
        m_cols = otherMatrix.m_cols;
        initMatrix();
    }

    for (int i = 0; i < m_rows; i++)
        for (int j = 0; j < m_cols; j++)
            m_data[i][j] = otherMatrix.m_data[i][j];

    return *this;
}

template <typename T>
T &MyMatrix<T>::operator()(int i, int j)
{
    assert(i >= 0);
    assert(i < m_rows);
    assert(j >= 0);
    assert(j < m_cols);

    return m_data[i][j];
}
