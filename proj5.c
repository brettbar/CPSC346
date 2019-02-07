/*
Class: CPSC 346-02
Team Member 1: Brett Barinaga
Team Member 2:
GU Username of project lead: bbarinaga
Pgm Name: proj5.c
Pgm Desc: Demonstrates the Peterson solution in multi-process communication
Usage: ./a.out or ./a.out # # # #
*/

#include <stdio.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

void parent(int*, int, int);
void child(int*, int, int);
void cs(char, int);
void non_cs(int);

int main(int argc, char** argv)
{ 
  int time_child = 0;
  int time_child_non_cs = 0;
  int time_parent = 0;
  int time_parent_non_cs = 0;

  char** args = argv;

  if (argc == 1)
  {
  	time_child = 1;
  	time_child_non_cs = 1;
  	time_parent = 5;
  	time_parent_non_cs = 5;
  }
  else if (argc == 5)
  {
  	time_child = atoi(args[1]);
  	time_child_non_cs = atoi(args[2]);
  	time_parent = atoi(args[3]);
  	time_parent_non_cs = atoi(args[4]);
  }
  else 
  {
  	fprintf(stderr, "wrong number of inputs given: acceptable uses: ./a.out or ./a.out # # # #");
  }

  int turn;
  int pr_0 = 0;
  int pr_1 = 0;

  int shmid;
  shmid = shmget(0,12,0777 | IPC_CREAT);
  int* address;
  address= (int*)shmat(shmid, 0,0);

  address[0] = turn; // Turn
  address[1] = pr_0; // pr_0
  address[2] = pr_1; // pr_1
  
  //check for proper arguments
  //fork here
  if(fork() == 0)
    child(address, time_child, time_child_non_cs);
  else 
    parent(address, time_parent, time_parent_non_cs);
}

void parent(int* address, int time_crit_sect, int time_non_crit_sect)
{
  for (int i = 0; i < 10; i++)
  {
   address[1] = 1; //pr_0 = 1;
   address[0] = 1; //turn = 1;

   while(address[2] && address[0]); // while(pr_1 && turn);
   //protect this
   cs('p', time_crit_sect);
   address[1] = 0; //pr_0 = 0;
   non_cs(time_non_crit_sect); 
  }
}

void child(int* address, int time_crit_sect, int time_non_crit_sect)
{
 for (int i = 0; i < 10; i++)
  {
  	address[2] = 1; // pr_1 = 1;
  	address[0] = 0; // turn = 0;

   while(address[1] && address[0]);// while(pr_0 && turn);
   //protect this
   cs('c', time_crit_sect);
   address[2] = 0; // pr_1 = 0;
   non_cs(time_non_crit_sect); 
  }
}

void cs(char process, int time_crit_sect)
{
 if (process == 'p')
  {
   printf("parent in critical section\n");
   sleep(time_crit_sect);
   printf("parent leaving critical section\n");
  }
 else
  {
   printf("child in critical section\n");
   sleep(time_crit_sect);
   printf("child leaving critical section\n");
  }
}

void non_cs(int time_non_crit_sect)
{
 sleep(time_non_crit_sect);
}