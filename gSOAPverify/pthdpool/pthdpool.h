#pragma once
#include <pthread.h>
#if __cplusplus <= 199711L
	#define null NULL
#else
	#define null nullptr
#endif
#ifdef __cplusplus
/* ���� C++ ��������ָ���� C ���﷨���� */
extern "C" {
#endif
	//�߳���������
	typedef struct my_task {
		void*			(*func)(void*);	/* ����ص�����ָ�� */
		void			*arg;			/* �����������Ĳ��� */
	} my_task_t;
	//�̳߳�����
	typedef struct my_pool {
		bool			dispose;		/* ��־�̳߳��Ƿ����� */
		int				thrd_num;		/* ����߳��� */
		int				queue_size;		/* �̶߳��г��� */
		int				cur_thrd_no;	/* �̳߳��л�߳��� */ 
		int				prev;			/* ����ͷ */
		int				tail;			/* ����β */
		pthread_t*		thrd_id;		/* �߳�ID������ָ�� */
		my_task_t*		queue;			/* �̶߳���ָ�� */
		pthread_mutex_t	queue_lock;		/* �̶߳����� */
		pthread_cond_t	queue_noti;		/* �߳�ͬ���������� */
	} my_pool_t;
	/*
	 * @brief     ��ʼ���̳߳�
	 * @param     thrd_num ����߳���
	 * @retval    �ɹ�-�������̳߳�ָ�룬ʧ��-���� 
	 */
	extern my_pool_t* my_pool_init(int thrd_num);
	/*
	 * @brief     ���̳߳��������
	 * @param     routine��������ָ�룬arg�����ݸ�routine�Ĳ���
	 * @retval    succeed: 0, failed: ����
	 */
	extern int my_pool_add_task(my_pool_t *pool, void*(*routine)(void*), void *arg);
	/*
	 * @brief     �ȴ�ĳ���̳߳�
	 * @param     Ҫ�ȴ����̳߳�ָ��
	 * @retval    none
	 */
	extern void my_pool_wait(my_pool_t *pool);
	/*
	 * @brief     �����̳߳�
	 * @param     none
	 * @retval    none
	 */
	extern int my_pool_destroy(my_pool_t *pool);
	
#ifdef __cplusplus
}
#endif
