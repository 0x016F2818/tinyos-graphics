#include "tgx_task.h"

static int n;
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t sem;

int on_post(void *context)
{
	return 0;
}

int on_task_complete(void *context, int err)
{
	printf("job %d done.\n", (int)context);
	n++;
	if (n == 10) sem_post(&sem);
	return 0;
}

int task_handler(void *context)
{
	int i, j, k;
	int m;
	for (i = 0; i < 500; i++)
		for (j = 0; j < 500; j++)
			for (k = 0; k < 500; k++)
				m = 1;
	return 0;

}

int main(int argc, char *argv[])
{
	int pid;
	pid = fork();

	tgx_task_schedule_t *t_task_sched = tgx_task_schedule_init(2);

	sem_init(&sem, 0, 0);

	int i;
	for (i = 0; i < 10; i++) {
		tgx_task_t *task = tgx_task_init();
		task->task_lock = NULL;
		task->on_post			= on_post;
		task->on_task_complete  = on_task_complete;
		task->task_handler		= task_handler;
		task->context = (void *)i;
		tgx_post_task(t_task_sched, task);
	}

	sem_wait(&sem);
	tgx_task_schedule_destroy(t_task_sched);

	return 0;
}
