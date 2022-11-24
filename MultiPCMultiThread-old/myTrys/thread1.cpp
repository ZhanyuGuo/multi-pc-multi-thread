#include <unistd.h>
#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 10
int count;

void* thread_func(void *arg) 
{
	count++;
	printf("count %d\n", count);
}

int main()
{
	pthread_t tid[MAX_THREADS];
	int i = 0;
	
	for (i = 0; i < MAX_THREADS; i++)
	{
		pthread_create(&tid[i], NULL, thread_func, NULL);	
	}

	sleep(1);
	
	return 0;
}
