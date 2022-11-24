#include <iostream>
using namespace std;

/*

    g++ -o test1.so -shared -fPIC hello.cpp

*/
void hello()
{
    cout << "hello world" << endl;
}

extern "C"
{
    void test()
    {
        hello();
    }
}