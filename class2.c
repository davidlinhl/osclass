#include <sched.h>
#include "sys/wait.h"
#include "sched.h"
#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
#include "semaphore.h"
#include "string.h"
#include "unistd.h"


int producer(void* args);
int consumer(void* args);
pthread_mutex_t mutex;
sem_t product;
sem_t warehouse;

char buffer[8][4];
int bp=0;
int main()
{
	pthread_mutex_init(&mutex,NULL);
	sem_init(&product,0,0);
	sem_init(&warehouse,0,8);
	int clone_flag,arg,retval;
	char* stack;
	clone_flag=CLONE_VM|CLONE_SIGHAND|CLONE_FS|CLONE_FILES;
	int i;
	for(i=0;i<2;i++)
	{
		arg=i;
		stack=(char*)malloc(4096);
		retval=clone((void*)producer,&(stack[4095]),clone_flag,(void*)&arg);
		stack=(char*)malloc(4096);
		retval=clone((void*)consumer,&(stack[4095]),clone_flag,(void*)&arg);
		sleep(1);
	}
	exit(1);
}

int producer(void * args)
{
  int id=*((int* )args);
  int i;
  for(i=0;i<10;i++)
  {
    sleep(i+1);
    sem_wait(&warehouse);
    pthread_mutex_lock(&mutex);
    if(id==0)
      strcpy(buffer[bp],"aaa\0");
    else
      strcpy(buffer[bp],"bbb\0");
    bp++;
    printf("producer %d produce %s in %d\n",id,buffer[bp-1],bp-1);
    pthread_mutex_unlock(&mutex);
    sem_post(&product);
  }
  printf("producer %d is over! \n",id);
}
int consumer(void * args)
{
  int id=*((int* )args);
  int i;
  for(i=0;i<10;i++)
  {
    sleep(10-i);
    sem_wait(&product);
    pthread_mutex_lock(&mutex);
    bp--;
    printf("consumer %d get %s in %d\n",id,buffer[bp],bp);
	strcpy(buffer[bp],"zzz\0");
	pthread_mutex_unlock(&mutex);
	sem_post(&warehouse);
	}
printf("consumer%d is over! \n",id);
}


// gcc class2.c -lpthread -D_GNU_SOURCE
