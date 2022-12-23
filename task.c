#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

/* TASK AND TASK QUEUES */

typedef struct task
{
	void* (*runnable)(void*);
	void* arg;
} task_t;

typedef struct task_node
{
	task_t task;
	struct task_node* prev, * next;
} task_node_t;

typedef struct task_queue
{
	task_node_t* head, * tail;
} task_queue_t;

static bool tasks_init(task_queue_t* tasks)
{
	task_node_t* head = (task_node_t*)malloc(sizeof(task_node_t));
	if (head == NULL)
		return false;
	task_node_t* tail = (task_node_t*)malloc(sizeof(task_node_t));
	if (tail == NULL)
	{
		free(head);
		return false;
	}

	head->next = tail;
	tail->prev = head;
	head->prev = tail->next = NULL;

	tasks->head = head;
	tasks->tail = tail;

	return true;
}

bool tasks_push(task_queue_t* tasks, task_t task)
{
	task_node_t* node = (task_node_t*)malloc(sizeof(task_node_t));
	if (node == NULL)
		return false;

	node->task = task;

	node->prev = tasks->tail->prev;
	node->next = tasks->tail;
	node->prev->next = node->next->prev = node;

	return true;
}

static bool tasks_is_empty(task_queue_t* tasks)
{
	return tasks->head->next == tasks->tail;
}

static bool tasks_pop(task_queue_t* tasks, task_t* p_task)
{
	if (tasks_is_empty(tasks))
		return false;

	task_node_t* node = tasks->head->next;
	node->prev->next = node->next;
	node->next->prev = node->prev;

	*p_task = node->task;
	free(node);

	return true;
}

static void tasks_destroy(task_queue_t* tasks)
{
	task_node_t* mov = tasks->head;
	while (mov != NULL)
	{
		task_node_t* next = mov->next;
		free(mov);
		mov = next;
	}
}

/* ASYNC_TASK_HANDLING */

#define THREAD_COUNT 6

static pthread_t threads[THREAD_COUNT];
static volatile bool keep_running;

static pthread_mutex_t mtx;
static pthread_cond_t cv;

static task_queue_t tasks;

bool async_schedule(task_t task)
{
	pthread_mutex_lock(&mtx);

	bool success = tasks_push(&tasks, task);

	if (success)
		pthread_cond_signal(&cv);

	pthread_mutex_unlock(&mtx);
	return success;
}

static bool async_deschedule(task_t* p_task)
{
	pthread_mutex_lock(&mtx);

	while (keep_running && !tasks_pop(&tasks, p_task))
		pthread_cond_wait(&cv, &mtx);

	pthread_mutex_unlock(&mtx);

	return keep_running;
}

/* THREADS AND CONCURRENCY */

static void* worker(void* arg)
{
	while (keep_running)
	{
		task_t task;
	        if (async_deschedule(&task))
			task.runnable(task.arg);
	}

	return NULL;
}

bool async_init()
{
	if (!tasks_init(&tasks))
		return false;

	pthread_mutex_init(&mtx, NULL);
	pthread_cond_init(&cv, NULL);

	keep_running = true;

	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_create(&threads[i], NULL, worker, NULL);

	return true;
}

void async_destroy()
{
	keep_running = false;
	pthread_cond_broadcast(&cv);

	for (int i = 0; i < THREAD_COUNT; i++)
		pthread_join(threads[i], NULL);

	pthread_cond_destroy(&cv);
	pthread_mutex_destroy(&mtx);

	tasks_destroy(&tasks);
}

/* TEST */
void do_sth(int asd)
{
	sleep(asd);
	printf("asd: %d\n", asd);
}

int main(void)
{
	async_init();

	int nums[5] = { 3, 1, 5, 2, 4 };

	for (int i = 0; i < 5; i++)
		async_schedule((task_t){ .runnable = do_sth, .arg = (void*)nums[i] });

	int c;
	while ((c = getchar()) != EOF && c != 'q')
		;

	async_destroy();

	return 0;
}
