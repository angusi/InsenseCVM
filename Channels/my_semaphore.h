#ifndef MY_SEMAPHORE_H
#define MY_SEMAPHORE_H

#include <pthread.h>


typedef struct my_sem {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	volatile int value;
} my_sem_t;


int my_sem_init(my_sem_t *sem, int value);
void my_sem_destroy(my_sem_t *sem);

void my_sem_wait(my_sem_t *sem);
void my_sem_post(my_sem_t *sem);
void binary_sem_wait(my_sem_t *sem);
void binary_sem_post(my_sem_t *sem);


#endif /* MY_SEMAPHORE_H */
