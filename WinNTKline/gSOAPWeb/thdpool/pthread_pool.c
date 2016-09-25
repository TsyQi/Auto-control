#include "pthread_pool.h"
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

my_pool_t* my_pool_init(int max_thr_num)
{
	my_pool_t *pool = (struct my_pool_t*) calloc(1, sizeof(pool));//�ڴ�Ķ�̬�洢���з���1������Ϊpool�������ռ�
	static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
	static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	if (max_thr_num < 1) 
		max_thr_num = 1;

	pthread_mutex_init(&pool_t.queue_lock, NULL);
	pthread_cond_init(&pool_t.queue_noti, NULL);
	return pool;
}

int my_pool_add_work(void *(*routine)(void *), void * arg)
{
	return 0;
}

void my_pool_destroy()
{
}

void my_pool_wait(my_pool_t * pool)
{
}
