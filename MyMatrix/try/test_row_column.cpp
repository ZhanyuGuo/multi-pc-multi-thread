#include <ctime>
#include <iostream>
using namespace std;

int main(int argc, char const *argv[])
{
    int **data;
    int size = 10000;
    clock_t start, end;
    double t;

    data = new int *[size];
    for (size_t i = 0; i < size; i++)
    {
        data[i] = new int[size];
    }

    start = clock();
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            data[i][j] = 100;
        }
    }
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    cout << t << endl;

    start = clock();
    for (size_t j = 0; j < size; j++)
    {
        for (size_t i = 0; i < size; i++)
        {
            data[i][j] = 100;
        }
    }
    end = clock();
    t = (double)(end - start) / CLOCKS_PER_SEC;
    cout << t << endl;

    return 0;
}
