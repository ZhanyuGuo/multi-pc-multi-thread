#include <iostream>

using namespace std;

int getMaxArray(int p[], int size);
int getMaxPtr(int *p, int size);

int main(int argc, char const *argv[])
{
    /* code */
    int a[] = {0, 1, 3, 4, 5, 6, 1, 4, 2, 1};
    int len = sizeof(a)/sizeof(int);
    int max;

    for (size_t i = 0; i < len; i++)
    {
        /* code */
        cout << a[i] << ' ';
    }
    cout << endl;
    max = getMaxPtr(a, len);
    cout << "max: " << max << endl;
    return 0;
}

int getMaxArray(int p[], int size)
{
    int max = p[0];
    for (size_t i = 1; i < size; i++)
    {
        /* code */
        if (p[i] > max) max = p[i];
    }

    return max;
}

int getMaxPtr(int *p, int size)
{
    int *np = p;
    int max = *np;
    for (; np < (p+size); np++)
    {
        /* code */
        if (*np > max) max = *np;
    }

    return max;
}