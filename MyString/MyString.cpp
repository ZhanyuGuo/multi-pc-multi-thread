#include "MyString.h"

int main(void)
{
    // 无参数构造
    MyString str1;
    cout << "str1 = " << str1 << endl;

    // char类型构造
    MyString str2 = "hello";
    cout << "str2 = " << str2 << endl;

    // MyString类型构造
    MyString str3 = str2;
    cout << "str3 = " << str3 << endl;

    // string类型构造
    string standardString = "world";
    MyString str4 = standardString;
    cout << "str4 = " << str4 << endl;

    // int类型构造
    MyString str5 = 123;
    cout << "str5 = " << str5 << endl;

    // 索引
    char ret1 = str2[2];
    cout << "ret1 = " << ret1 << endl;
    char ret2 = str2[6]; // 越界返回NULL
    cout << "ret2 = " << ret2 << endl;

    // +重载
    str5 = str5 + 456;
    cout << "after add: str5 = " << str5 << endl;

    // +=重载
    str5 += 789;
    cout << "after adde: str5 = " << str5 << endl;

    // subString，切片
    str4 = str4.subString(1, 3);
    cout << "after subString: "
         << "str4 = " << str4 << endl;

    // toInt，字符到数字转换
    int x1 = str5.toInt();
    cout << "after toInt: x1 = " << x1 << endl;
    int x2 = str2.toInt(); // 不纯为数字的返回0（作业要求为-1）
    cout << "after toInt: x2 = " << x2 << endl;

    // cin重载
    cout << "input str1: ";
    cin >> str1;
    cout << "after input: "
         << "str1 = " << str1 << endl;

    // system("pause");
    return 0;
}