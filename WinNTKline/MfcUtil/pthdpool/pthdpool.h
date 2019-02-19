#pragma once
#include <pthread.h>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <cerrno>
#if __cplusplus <= 199711L
#define null NULL
#else
#define null nullptr
#endif
#ifdef __cplusplus
/* 对于 C++ 编译器，指定用 C 的语法编译 */
extern "C" {
#endif
    //线程任务属性
    typedef struct my_task {
        void*              (*func)(void*);   /* 任务回调函数指针 */
        void               *arg;             /* 传入任务函数的参数 */
    } my_task_t;
    //线程池属性
    typedef struct my_pool {
        bool               dispose;        /* 标志线程池是否销毁 */
        int                thrd_num;       /* 最大线程数 */
        int                queue_size;     /* 线程队列长度 */
        int                cur_thrd_no;    /* 线程池中活动线程数 */
        int                prev;           /* 队首编号 */
        int                tail;           /* 队尾编号 */
        pthread_t*         thrd_id;        /* 线程ID数组首指针 */
        my_task_t*         queue;          /* 线程队列指针 */
        pthread_mutex_t    queue_lock;     /* 线程队列锁 */
        pthread_cond_t     queue_noti;     /* 线程同步条件变量 */
    } my_pool_t;
    /**
      * @brief     初始化线程池
      * @param     thrd_num:最大线程数,sz_que:队列长度
      * @retval    成功-创建的线程池指针，失败-其他
      */
    extern my_pool_t* my_pool_init(int thrd_num, int sz_que);
    /**
      * @brief     向线程池添加任务
      * @param     pool: 添加任务的线程池; routine: 任务函数指针; arg: 传递给routine的参数
      * @retval    0,成功; 其他,失败
      */
    extern int my_pool_add_task(my_pool_t *pool, void*(*routine)(void*), void *arg);
    /**
      * @brief     等待某个线程池
      * @param     pool,要等待结束的线程池指针
      * @retval    0,成功; 其他,失败
      */
    extern int my_pool_wait(my_pool_t *pool);
    /**
      * @brief     销毁线程池
      * @param     pool,要销毁的线程池
      * @retval    0,成功; 其他,失败
      */
    extern int my_pool_destroy(my_pool_t *pool);

#ifdef __cplusplus
}
#endif
