#include "my_mutex.h"

int my_mutex_init(my_mutex_t *mutex){
	mutex->num_acquired = 0;
	mutex->thread = (pthread_t) 0;
	return pthread_mutex_init(&mutex->pmutex, NULL);
}

void my_mutex_destroy(my_mutex_t *mutex){
	pthread_mutex_lock(&mutex->pmutex);
	mutex->num_acquired = 0;
	mutex->thread = (pthread_t) 0;
	pthread_mutex_unlock(&mutex->pmutex);
	pthread_mutex_destroy(&mutex->pmutex);
}

void my_mutex_lock(my_mutex_t *mutex){
	pthread_t current_thread = pthread_self();
	if(mutex->num_acquired > 0 && mutex->thread == current_thread){
		mutex->num_acquired++;
		return;
	}
	pthread_mutex_lock(&mutex->pmutex);
	mutex->thread = current_thread;
	mutex->num_acquired++;
}

void my_mutex_unlock(my_mutex_t *mutex){
	pthread_t current_thread = pthread_self();
	if(mutex->num_acquired > 1 && mutex->thread == current_thread){
		mutex->num_acquired--;
		return;
	}
	mutex->thread = (pthread_t) 0;
	mutex->num_acquired--;
	pthread_mutex_unlock(&mutex->pmutex);
}
