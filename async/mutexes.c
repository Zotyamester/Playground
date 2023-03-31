#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define CAPACITY 50
#define TRIGGER_LINE_FEED 60

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

int counter = 0;
int line_length = 0;

void* increment_counter_n_times(void* arg)
{
	int n = (int)arg; /* quirky things, but done intentionally */
	
	for (int i = 0; i < n; i++)
	{
		pthread_mutex_lock(&mtx);

		counter++;

		line_length += printf("%d ", counter);
		if (line_length >= TRIGGER_LINE_FEED)
		{
			putchar('\n');
			line_length = 0;
		}

		pthread_mutex_unlock(&mtx);
	}

	return NULL;
}

int main(void)
{
	int total_to_reach;
	if (scanf("%d", &total_to_reach) != 1 || total_to_reach < 0)
		return 1;

	int count_of_threads = (total_to_reach + CAPACITY - 1) / CAPACITY;
	pthread_t* tids = (pthread_t*)malloc(count_of_threads * sizeof(pthread_t));
	if (tids == NULL)
		return 2;

	for (int i = 0; i < count_of_threads; i++)
	{
		int n = (total_to_reach > CAPACITY) ? CAPACITY : total_to_reach;
		pthread_create(&tids[i], NULL, increment_counter_n_times, (void*)n);
	}

	for (int i = 0; i < count_of_threads; i++)
		pthread_join(tids[i], NULL);

	printf("\nCount at the end: %d\n", counter);

	return 0;
}
