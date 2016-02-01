#ifndef MY_MUTEX_H
#define MY_MUTEX_H

#include <pthread.h>


typedef struct my_mutex {
	pthread_mutex_t pmutex;
	pthread_t thread;
	unsigned num_acquired;
} my_mutex_t;


int my_mutex_init(my_mutex_t *mutex);
void my_mutex_destroy(my_mutex_t *mutex);

void my_mutex_lock(my_mutex_t *mutex);
void my_mutex_unlock(my_mutex_t *mutex);

#endif /* MY_MUTEX_H */
