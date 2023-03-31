#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define SEED 69
#define N 1000
#define RAND_MOD 100000000

#define NO_THREADS 12

typedef struct queue
{
	int data;
	struct queue* next;
} queue;

void queue_free(queue** pqueue)
{
	queue* mov = *pqueue;
	while (mov != NULL)
	{
		queue* next = mov->next;
		free(mov);
		mov = next;
	}
	*pqueue = NULL;
}

void queue_push(queue** pqueue, int data)
{
	queue* node = (queue*)malloc(sizeof(queue));
	if (node == NULL)
		return;
	node->data = data;
	node->next = NULL;
	while (*pqueue != NULL)
		pqueue = &(*pqueue)->next;
	*pqueue = node;
}

int queue_pop(queue** pqueue)
{
	queue* head = *pqueue;
	if (head == NULL)
		return -1;
	int data = head->data;
	*pqueue = head->next;
	free(head);
	return data;
}

/* just some naive implementation to keep each
 * thread running for a little while */
bool is_prime(int n)
{
	for (int div = 2; div < n; div++)
		if (n % div == 0)
			return false;
	return true;
}

pthread_cond_t cvr;
pthread_mutex_t mtx;

void* worker(void* arg)
{
	queue** pqueue = (queue**)arg;
	int tid = pthread_self();

	for (;;)
	{
		pthread_mutex_lock(&mtx);

		int data;
		while ((data = queue_pop(pqueue)) == -1)
		{
			pthread_cond_wait(&cvr, &mtx);
		}

		pthread_mutex_unlock(&mtx);

		if (data == -2)
			break;

		printf("Thread %d: %d is%s a prime\n", tid, data, is_prime(data) ? "" : " not");
	}
	
	return NULL;
}

int main(void)
{
	srand(SEED);

	queue* queue = NULL;

	pthread_cond_init(&cvr, NULL);
	pthread_mutex_init(&mtx, NULL);

	pthread_t threads[NO_THREADS];

	for (int i = 0; i < NO_THREADS; i++)
		pthread_create(&threads[i], NULL, worker, (void*)&queue);

	printf("All the %d of my buddies have gone on their ways. :D\n", NO_THREADS);

	for (int i = 0; i < N + NO_THREADS; i++)
	{
		pthread_mutex_lock(&mtx);

		int data = (i < N) ? rand() % RAND_MOD : -2;
		queue_push(&queue, data);
		pthread_cond_signal(&cvr);

		pthread_mutex_unlock(&mtx);
	}

	for (int i = 0; i < NO_THREADS; i++)
		pthread_join(threads[i], NULL);

	pthread_cond_destroy(&cvr);
	pthread_mutex_destroy(&mtx);

	queue_free(&queue);

	return 0;
}
