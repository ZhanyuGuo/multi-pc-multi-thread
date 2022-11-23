// #pragma warning(disable:4996)
#include <iostream>
#include <string.h>

#define NUM 128
using namespace std;

class MyString
{
public:
    MyString(void);
    MyString(const char *str);
    MyString(const MyString *str);
    MyString(string str);
    MyString(int x);

    char operator[](size_t index);
    friend ostream &operator<<(ostream &out, const MyString &obj);
    friend istream &operator>>(istream &in, const MyString &obj);
    MyString &operator+(const MyString &toAdd);
    MyString &operator+=(const MyString &toAdde);
    MyString subString(size_t start, size_t length);
    int toInt(void);

private:
    size_t _length;
    char *_str;
};

MyString::MyString(void)
{
    _length = 0;
    _str = (char *)malloc(_length + 1);
    _str[1] = '\0';
}

MyString::MyString(const char *str)
{
    _length = strlen(str);
    _str = (char *)malloc(_length + 1);
    strcpy(_str, str);
}

MyString::MyString(const MyString *str)
{
    _length = str->_length;
    _str = (char *)malloc(_length + 1);
    strcpy(_str, str->_str);
}

MyString::MyString(string str)
{
    _length = str.size();
    _str = (char *)malloc(_length + 1);
    strcpy(_str, str.c_str());
}

MyString::MyString(int x)
{
    char *buf = (char *)malloc(NUM);
    // itoa(x, buf, 10);
    sprintf(buf, "%d", x);
    _length = strlen(buf);
    _str = (char *)malloc(_length + 1);
    strcpy(_str, buf);
    free(buf);
}

char MyString::operator[](size_t index)
{
    if (index < _length)
        return _str[index];
    return NULL;
}

ostream &operator<<(ostream &out, const MyString &obj)
{
    out << obj._str;
    return out;
}

istream &operator>>(istream &in, const MyString &obj)
{
    in >> obj._str;
    // has a bug: how to assign _length ?
    return in;
}

MyString &MyString::operator+(const MyString &toAdd)
{
    _length += toAdd._length;
    strcat(_str, toAdd._str);
    return *this;
}

MyString &MyString::operator+=(const MyString &toAdde)
{
    *this = *this + toAdde;
    return *this;
}

MyString MyString::subString(size_t start, size_t length)
{
    MyString ret;
    string a = _str;
    ret = a.substr(start, length);
    return ret;
}

int MyString::toInt(void)
{
    return atoi(_str);
}
