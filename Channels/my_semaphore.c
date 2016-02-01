#include "my_semaphore.h"

int my_sem_init(my_sem_t *sem, int value){
	sem->value = value;
	if( (pthread_cond_init(&sem->cond, NULL)) == -1)
		return -1;
	return pthread_mutex_init(&sem->mutex, NULL);
}

void my_sem_destroy(my_sem_t *sem){
	pthread_mutex_lock(&sem->mutex);
	pthread_cond_destroy(&sem->cond);
	sem->value = -1;
	pthread_mutex_unlock(&sem->mutex);
	pthread_mutex_destroy(&sem->mutex);
}


void my_sem_wait(my_sem_t *sem){
	pthread_mutex_lock(&sem->mutex);
	if(sem->value <= 0){
		pthread_cond_wait(&sem->cond, &sem->mutex);
	}
	sem->value--;
	pthread_mutex_unlock(&sem->mutex);
}

void my_sem_post(my_sem_t *sem){
	pthread_mutex_lock(&sem->mutex);
	sem->value++;
	pthread_cond_signal(&sem->cond);
	pthread_mutex_unlock(&sem->mutex);
}

void binary_sem_wait(my_sem_t *sem){
	pthread_mutex_lock(&sem->mutex);
	if(sem->value == 0){
		pthread_cond_wait(&sem->cond, &sem->mutex);
	}
	sem->value = 0;
	pthread_mutex_unlock(&sem->mutex);
}


void binary_sem_post(my_sem_t *sem){
	pthread_mutex_lock(&sem->mutex);
	sem->value = 1;
	pthread_cond_signal(&sem->cond);
	pthread_mutex_unlock(&sem->mutex);
}
