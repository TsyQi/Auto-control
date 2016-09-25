#pragma once
#include <pthread.h>

	//�߳���������
	typedef struct my_pool_work {
		void*               (*func)(void*);			/* ����ص�����ָ�� */
		void                *arg;                   /* �����������Ĳ��� */
		struct my_pool_work   *prev;				/* ����ͷָ�� */
		struct my_pool_work   *next;				/* ����βָ�� */
	} my_pool_work_t;
	//�̳߳�����
	typedef struct my_pool {
		int             dispose;					/* ��־�̳߳��Ƿ����� */
		int             max_thr_num;                /* ����߳��� */
		int				cur_thr_num;				/* �̳߳��л�߳��� */ 
		int				idl_thr_num;				/* �̳߳��п����߳��� */  
		pthread_t*		thr_id;                     /* �߳�ID���� */
		my_pool_work_t*	queue_head;                 /* �̶߳���ͷ */
		my_pool_work_t*	queue_next;                 /* �̶߳���β */
		pthread_mutex_t queue_lock;					/* �̶߳����� */
		pthread_cond_t  queue_noti;					/* �������� */
	} my_pool_t;

	/*
	 * @brief     ��ʼ���̳߳�
	 * @param     max_thr_num ����߳���
	 * @retval    �ɹ�-0��ʧ��-���� 
	 */
	extern my_pool_t* my_pool_init(int max_thr_num);
	/*
	 * @brief     ���̳߳��������
	 * @param     routine��������ָ�룬arg�����ݸ�routine�Ĳ���
	 * @retval    succeed��0��failed������
	 */
	extern int my_pool_add_work(void*(*routine)(void*), void *arg);
	/*
	 * @brief     �����̳߳�
	 * @param     none
	 * @retval    none
	 */
	extern void my_pool_destroy();

	extern void my_pool_wait(my_pool_t *pool);

	my_pool_t pool_t;

