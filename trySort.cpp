#include <iostream>
#include <stdio.h>

using namespace std;
void getSort(int *p, int len);
void show(int *p, int len);
void qsort(double s[], int l, int r);
double NewSort(const double data[], const int len, double result[]);

int main(int argc, char const *argv[])
{
    /* code */
    int a[] = {6, 1, 2, 5, 9, 2, 2, 9};
    double b[] = {1.5, 2.3, 1.4, 3.3, 5.6, 7.0, 2.2};
    int alen = sizeof(a)/sizeof(int);
    const int blen = sizeof(b)/sizeof(double);

    // show(a, alen);
    // // getSort(a, len);
    // qsort(a, 0, alen - 1);
    // show(a, alen);
    
    for (int i = 0; i < blen; i++)
    {
        printf("%lf\r\n", b[i]);
    }
    printf("\r\n");
    double result[blen];
    NewSort(b, blen, result);
    for (int i = 0; i < blen; i++)
    {
        printf("%lf\r\n", result[i]);
    }
    return 0;
}

double NewSort(const double data[], const int len, double result[])
{
    int l = 0, r = len - 1;
    for (int i = 0; i < len; i++)
    {
        result[i] = data[i];
    }
    qsort(result, l, r);
}

void qsort(double s[], int l, int r)
{
    if (l < r)
    {
        int i = l, j = r;
        double x = s[i];
        while (i < j)
        {
            while (i < j && s[j] >= x)
            {
                j--;
            }
            if (i < j)
            {
                s[i++] = s[j];
            }
            while (i < j && s[i] < x)
            {
                i++;
            }
            if (i < j)
            {
                s[j--] = s[i];
            }           
        }
        s[i] = x;
        qsort(s, l, i - 1);
        qsort(s, i + 1, r);
    }
}

void getSort(int *p, int len)
{
    int *np = p;
    int t;
    for (size_t i = 0; i < len; i++)
    {
        /* code */
        for (; np < (p + len - 1); np++)
        {
            /* code */
            if (*np < *(np + 1))
            {
                t = *np;
                *np = *(np + 1);
                *(np + 1) = t;
            }
        }
        np = p;
    }
}

void show(int *p, int len)
{
    int *np = p;
    for (; np < (p + len); np++)
    {
        /* code */
        cout << *np << ' ';
    }

    cout << endl;
}