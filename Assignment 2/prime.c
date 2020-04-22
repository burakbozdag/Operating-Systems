// Development environment: Ubuntu 16.04.6 LTS (Xenial Xerus)
// To compile: gcc prime.c -o prime -pthread
// To run: ./prime interval_min interval_max np nt
// Example: ./prime 101 200 2 2

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct Data{
	int min;
	int max;
	int pro;
	int thr;
	int* primes;
}Data;

void* findPrimes(void* d) {
	Data* data = (Data*)d;
	printf("Thread %d.%d: searching in %d-%d\n",
	data->pro, data->thr, data->min, data->max);
	int num;
	for (num = data->min; num <= data->max; ++num) {
		int c;
		int prime = 1;
		for (c = 2; c < num; ++c) {
			if (num % c == 0) {
				prime = 0;
				break;
			}
		}
		if (prime) { //Add the number to the shared memory
			int p = 0;
			while (data->primes[p] != 0) {
				++p;
			}
			data->primes[p] = num;
		}
	}
	
	return 0;
}

int main(int argc, char** argv) {
  pid_t child_pid;
  int status = 0;
  
  printf("Master: Started.\n");
	int interval_min = atoi(argv[1]);
  int interval_max = atoi(argv[2]);
  int np = atoi(argv[3]);
  int nt = atoi(argv[4]);
  if (np * nt > interval_max - interval_min + 1) {
  	printf("Too many threads for these numbers! Max allowed process*thread=%d\n",
		interval_max - interval_min + 1);
		return 1;
  }
  //printf("%d %d %d %d\n", interval_min, interval_max, np, nt);
  
  int intervalCount = (interval_max - interval_min + 1) / np;
  int start[np]; //Interval start numbers
  int i;
  for (i = 0; i < np; ++i) {
  	start[i] = interval_min + (i * intervalCount);
  }
  
  key_t key = ftok("shmfile", 65);
  int shmid = shmget(key, sizeof(int[intervalCount]), 0666|IPC_CREAT);
  int* primeNumbers = (int*)shmat(shmid, (void*)0, 0); //Shared memory
  for (i = 0; i < intervalCount; ++i) { //Initializing shared memory
  	primeNumbers[i] = 0;
  }
  
  for (i = 0; i < np; ++i) { //Creating processes
    if ((child_pid = fork()) == 0) {
      printf("Slave %d: Started. Interval %d-%d\n",
			i + 1, start[i], start[i] + intervalCount - 1);
			
      pthread_t threads[nt];
      void* retvals[nt];
      
      int thrIntCount = (interval_max - interval_min + 1) / (np * nt);
      int thrStart[nt]; //Interval start numbers for threads
      int t;
      for (t = 0; t < nt; ++t) {
      	thrStart[t] = start[i] + (t * thrIntCount);
      }
      
      int j;
      for (j = 0; j < nt; ++j) { //Creating threads
				Data data;
				data.min = thrStart[j];
				data.max = thrStart[j] + thrIntCount - 1;
				data.pro = i + 1;
				data.thr = j + 1;
				data.primes = primeNumbers;
				pthread_create(&threads[j], NULL, findPrimes, &data);
				pthread_join(threads[j], &retvals[j]);
      }
      
      printf("Slave %d: Done.\n", i + 1);
      shmdt(primeNumbers);
      exit(0);
    }
  }
  
  waitpid(child_pid, &status, 0); //Waiting for all processes
  printf("Master: Done. Prime numbers are: ");
  for (i = 0; i < intervalCount && primeNumbers[i] != 0; ++i) {
  	printf("%d, ", primeNumbers[i]);
  }
  printf("\n");
  shmdt(primeNumbers);
  shmctl(shmid, IPC_RMID, NULL);

  return 0;
}
