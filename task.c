#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

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
	task_node_t* headtail;
} task_queue_t;

static bool tasks_init(task_queue_t* tasks)
{
	task_node_t* headtail = (task_node_t*)malloc(sizeof(task_node_t));
	if (headtail == NULL)
		return false;

	task_t null_task = { .runnable = NULL, .arg = NULL };
	headtail->task = null_task;

	headtail->prev = headtail->next = headtail;

	tasks->headtail = headtail;

	return true;
}

bool tasks_push(task_queue_t* tasks, task_t task)
{
	task_node_t* node = (task_node_t*)malloc(sizeof(task_node_t));
	if (node == NULL)
		return false;

	node->task = task;

	node->prev = tasks->headtail->prev;
	node->next = tasks->headtail;
	node->prev->next = node->next->prev = node;

	return true;
}

static bool tasks_is_empty(task_queue_t* tasks)
{
	return tasks->headtail->next == tasks->headtail;
}

static bool tasks_pop(task_queue_t* tasks, task_t* p_task)
{
	if (tasks_is_empty(tasks))
		return false;

	task_node_t* node = tasks->headtail->next;
	node->prev->next = node->next;
	node->next->prev = node->prev;

	*p_task = node->task;
	free(node);

	return true;
}

static void tasks_destroy(task_queue_t* tasks)
{
	task_node_t* mov = tasks->headtail->next;
	while (mov != tasks->headtail)
	{
		task_node_t* next = mov->next;
		free(mov);
		mov = next;
	}
	free(tasks->headtail);
}

/* ASYNC_TASK_HANDLING */

static pthread_t* threads;
static size_t thread_count;
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
	        if (async_deschedule(&task) && task.runnable != NULL)
			task.runnable(task.arg);
	}

	return NULL;
}

bool async_init(size_t new_thread_count)
{
	thread_count = new_thread_count;
	threads = (pthread_t*)malloc(thread_count * sizeof(pthread_t));
	if (threads == NULL)
		goto error_at_thread_malloc;

	if (!tasks_init(&tasks))
		goto error_at_task_init;

	if (pthread_mutex_init(&mtx, NULL) != 0)
		goto error_at_mutex_init;
	if (pthread_cond_init(&cv, NULL) != 0)
		goto error_at_cond_init;

	keep_running = true;

	size_t i;
	for (i = 0; i < thread_count; i++)
		if (pthread_create(&threads[i], NULL, worker, NULL) != 0)
			goto error_at_thread_create;

	return true;

error_at_thread_create:
	for (size_t j = 0; j < i; j++)
		pthread_kill(threads[j], 0);
error_at_cond_init:
	pthread_cond_destroy(&cv);
error_at_mutex_init:
	pthread_mutex_destroy(&mtx);
error_at_task_init:
	free(threads);
error_at_thread_malloc:
	return false;
}

void async_destroy(bool force)
{
	keep_running = false;
	pthread_cond_broadcast(&cv);

	for (size_t i = 0; i < thread_count; i++)
		if (force)
			pthread_kill(threads[i], 0);
		else
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
	if (!async_init(6))
	{
		perror("Oh, no");
		return 1;
	}

	int nums[5] = { 3, 1, 5, 2, 4 };

	for (int i = 0; i < 5; i++)
		async_schedule((task_t){ .runnable = do_sth, .arg = (void*)nums[i] });

	int c;
	while ((c = getchar()) != EOF && c != 'q')
		;

	async_destroy(false);

	return 0;
}
