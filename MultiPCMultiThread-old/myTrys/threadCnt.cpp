#include <pthread.h>
#include <stdio.h>

void* foo(void* arg)
{
    while (1)
    {
    }
    
}

int main()
{
    int i = 0;
    pthread_t thread;
    while (1) 
    {
        if (pthread_create(&thread, NULL, foo, NULL) != 0)
        {
            break;
        }
        i++;
        printf("i = %d\n", i);
    }
    return 0;
}
