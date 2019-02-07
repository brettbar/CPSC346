/*
Class:CPSC 346-02
Team Member 1: Sebastian Berven
Team Member 2: Brett Barinaga 
GU Username of project lead: sberven
Pgm Name: Homework 7
Pgm Desc: Uses a pipe to communicate between different threads, creating random numbers and determining if they're prime. 
Usage: ./a.out 10 (how many numbers should be created.)
*/


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>


int isPrime(int);
int generateRandomNumber();
void *reader(void*);
void *writer(void*);


#define LOW 0
#define HIGH 99999

struct threadpass {
	int* pipe;
	int value;
};

typedef struct threadpass passer;

int main(int argc, char* argv[])
{
	int fd[2]; // fd[0] is setup for reading, fd[1] is set up for writing 
	struct threadpass vessel;
	int status;

	pthread_t tid1, tid2, tid3, tid4;

	int numInts = 10; // Default if no value specified will be 10

	if (argc > 2) 
	{
		fprintf(stderr, "Invalid number of arguments\n");
		return 0;
	}
	if (argc == 2)
		numInts = atoi(argv[1]);

	pipe(fd);

	vessel.pipe = fd;
	vessel.value = numInts;

	
	status = pthread_create(&tid1, NULL, writer, (void*)&vessel);
	if (status != 0)
	{
		printf("Error in thread 1: %d\n", status);
		exit(-1);
	}

	status = pthread_create(&tid2, NULL, reader, (void*)&vessel);
	if (status != 0)
	{
		printf("Error in thread 2: %d\n", status);
		exit(-1);
	}
	
	status = pthread_create(&tid3, NULL, reader, (void*)&vessel);
	if (status != 0)
	{
		printf("Error in thread 3: %d\n", status);
		exit(-1);
	}
	
	status = pthread_create(&tid4, NULL, reader, (void*)&vessel);
	if (status != 0)
	{
		printf("Error in thread 4: %d\n", status);
		exit(-1);
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);
	pthread_join(tid3, NULL);
	pthread_join(tid4, NULL);

	return 0;
}

int isPrime(int num)
{
 int i = 2;
 while (i < num)
 {
  if (num % i == 0)
     return 0;
  ++i;
 }
 return 1;
} 

int generateRandomNumber()
{
	srand ( time(0) );
	return rand() % (HIGH + 1 - LOW) + LOW;
}

void *reader(void *vessel) 
{
	passer* readParams = (passer*)vessel;
	int result, num,tf;
	int args = readParams->value;
	for (int i = 0; i < (args/3); i++)
	{
		result = read(readParams->pipe[0], &num, sizeof(num));
		if (result < 0)
			fprintf(stderr, "reader error\n");
		tf = isPrime(num);
		if(tf == 1)
			printf("%d is prime.\n", num);
	}
	//printf("reader is called\n");
}

void *writer(void *vessel)
{
	passer* readParams = (passer*)vessel;
	int result;
	int args = readParams->value;

	for (int i = 0; i < args; i++)
	{
		int temp = generateRandomNumber();
		printf("%d was generated.\n", temp);
		result = write(readParams->pipe[1], &temp, sizeof(temp));
		sleep(1);
		if (result < 1)
			fprintf(stderr, "writer error\n");
	}

}
