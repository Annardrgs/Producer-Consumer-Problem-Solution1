#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

pthread_t *producers;
pthread_t *consumers;
sem_t buf_mutex,empty_count,fill_count;

int *buf,buf_pos=-1,buf_len; 													// Declaração de todas as variaveis ate aqui


int produce(pthread_t self){
	int p = 1 + rand()%40;
	printf("Producer %d produced %d \n",1,p);
	return p;
}

void consume(int p,pthread_t self){
	int cont = 0;
	printf("Buffer:");
	while (cont<=buf_pos){
		printf("%d ",*(buf+cont));
		++cont;
	}
	printf("\nConsumer %d consumed %d \nCurrent buffer lenght: %d\n",1,p,buf_pos);
}

void* producer(void *args){
	while(1){
		sem_wait(&empty_count);
		sem_wait(&buf_mutex);
		int p = produce(pthread_self());
		++buf_pos;			
		*(buf + buf_pos) = p; 
		sem_post(&buf_mutex);
		sem_post(&fill_count);
		sleep(1 + rand()%3);
	}
	return NULL;
}

void* consumer(void *args){
	int c;
	while(1){
		sem_wait(&fill_count);
		sem_wait(&buf_mutex);
		c = *(buf+buf_pos);
		consume(c,pthread_self());
		--buf_pos;
		sem_post(&buf_mutex);
		sem_post(&empty_count);
		sleep(1+rand()%5);
	}
	return NULL;
}

int main(void){
	int i,err;
	srand(time(NULL));
	sem_init(&buf_mutex,0,1);														// inicia semaforo do mutex
	sem_init(&fill_count,0,0);														// inicia o semaforo do buffer cheio
	producers = (pthread_t*) malloc(sizeof(pthread_t));								// aloca memoria pro producer
	consumers = (pthread_t*) malloc(sizeof(pthread_t));								// aloca memoria pro consumer
	buf_len = 1;
	buf = (int*) malloc(buf_len*sizeof(int));										// aloca pro buffer
	sem_init(&empty_count,0,buf_len);												// incia semaforo do buffer vazio
	err = pthread_create(producers+i,NULL,&producer,NULL); 							// cria e starta a thread producer
	if(err != 0){ 																	// verifica se tem erro
		printf("Error creating producer %d: %s\n",i+1,strerror(err));
	}else{
		printf("Successfully created producer %d\n",i+1);
	}
	err = pthread_create(consumers+i,NULL,&consumer,NULL); 							// cria e starta a thread consumer
	if(err != 0){ 																	// verifica se tem erro
		printf("Error creating consumer %d: %s\n",i+1,strerror(err));
	}else{
		printf("Successfully created consumer %d\n",i+1);
	}
	pthread_join(*(producers+i),NULL);												// espera a outra thread acabar e passa o producer
	pthread_join(*(consumers+i),NULL);												// espera a thread acabar e passa o consumer
	return 0;
}
