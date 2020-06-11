#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int fibNext(int n) {
  return (int)round(n * ((1 + sqrt(5)) / 2.0));
}

void semWait(int* sem) {
  while (*sem == 0);
  *sem = 0;
}

void semPost(int* sem) {
  *sem = 1;
}

int main(int argc, char** argv) {
  pid_t child_pid;
  int status = 0;
  //sem_t* smutex; //Semaphores
  //sem_t* slock;

  int N, ni, nd, ti, td;
  N = atoi(argv[1]);	//The amount of money that needs to be stored in the
                        // moneybox before decreaser processes start working.
											
  ni = atoi(argv[2]);	//Number of increaser processes.
	
  nd = atoi(argv[3]);	//Number of decreaser processes.
	
  ti = atoi(argv[4]);	//Amount of turns the increaser processes will run
                        // before decreasers start working.
											
  td = atoi(argv[5]);	//Amount of turns the decreaser processes will run
                        // before increasers start working.
	
  if (ni % 2 != 0 || nd % 2 != 0) { //Error checking
    printf("\nInvalid arguments.\n");
    return 1;
  }

  printf("Master Process: Current money is 0\n");

  key_t key = ftok("shmfile", 65);
  int shmid = shmget(key, sizeof(int[4]), 0666|IPC_CREAT);
  int* memory = (int*)shmat(shmid, (void*)0, 0); //Shared memory pointer
  memory[0] = 0; //Moneybox
  memory[1] = 0; //Finish flag
  memory[2] = 1; //Lock unlocked
  memory[3] = 1; //Mutex unlocked
  int* moneybox = &memory[0];
  int* finished = &memory[1]; //Finish signal
  int* lock = &memory[2];
  int* mutex = &memory[3];
  int turn = 1;

  //smutex = sem_open("semfile", O_CREAT | O_EXCL, 0644, 0); //Process separator
  //slock = sem_open("lckfile", O_CREAT | O_EXCL, 0644, 0); //General separator

  if ((child_pid = fork()) == 0) { //Increaser manager
    while (*finished == 0) {
      semWait(lock);
      for (int t = 0; t < ti; ++t) {
        for (int i = 0; i < ni; ++i) {
          if ((child_pid = fork()) == 0) { //Increaser processes
            semWait(mutex);
            if (*finished == 1) {
              semPost(mutex);
              exit(0);
            }
            if (i % 2 == 0) { //+10 increaser
              *moneybox += 10;
              if (*finished) {
                semPost(mutex);
                exit(0);
              }
              printf("Increaser Process %d: Current money is %d", i, *moneybox);
            } else { //+15 increaser
              *moneybox += 15;
              if (*finished) {
                semPost(mutex);
                exit(0);
              }
              printf("Increaser Process %d: Current money is %d", i, *moneybox);
            }
            semPost(mutex);
            printf("\n");
            exit(0);
          }
        }
        waitpid(child_pid, &status, 0); //Waiting for processes
        semWait(mutex);
        if (*finished) {
          semPost(mutex);
          exit(0);
        }
        printf("Increaser processes finished their turn %d", turn++);
        semPost(mutex);
        printf("\n");
      }
      semPost(lock);
      printf("\n");
    }

    exit(0);
  }

  int fib = 0;

  if ((child_pid = fork()) == 0) { //Decreaser manager
    while (*moneybox < N);
    while (*finished == 0) {
      semWait(lock);
      for (int t = 0; t < td; ++t) {
        for (int i = 0; i < nd; ++i) {
          if ((child_pid = fork()) == 0) { //Decreaser processes
            semWait(mutex);
            if (*finished == 1) {
              semPost(mutex);
              exit(0);
            }
            if (i % 2 == 0) { //Even decreaser
              if (*moneybox % 2 == 1) {
                semPost(mutex);
                exit(0);
              }
              if (*moneybox < fib) {
                if (*finished) {
                  semPost(mutex);
                  exit(0);
                }
                printf("Decreaser Process %d: Current money is less than %d, signaling master to finish\n", i, fib);
                *finished = 1;
                semPost(mutex);
                exit(0);
              }
              if (!fib)
                *moneybox -= 1;
              else
                *moneybox -= fib;
              if (*finished) {
                semPost(mutex);
                exit(0);
              }
              printf("Decreaser Process %d: Current money is %d\n", i, *moneybox);
            } else { //Odd decreaser
              if (*moneybox % 2 == 0) {
                semPost(mutex);
                exit(0);
              }
              if (*moneybox < fib) {
                if (*finished) {
                  semPost(mutex);
                  exit(0);
                }
                printf("Decreaser Process %d: Current money is less than %d, signaling master to finish\n", i, fib);
                *finished = 1;
                semPost(mutex);
                exit(0);
              }
              if (!fib)
                *moneybox -= 1;
              else
                *moneybox -= fib;
              if (*finished) {
                semPost(mutex);
                exit(0);
              }
              printf("Decreaser Process %d: Current money is %d\n", i, *moneybox);
            }
            semPost(mutex);
            exit(0);
          }
        }
        waitpid(child_pid, &status, 0); //Waiting for processes
        semWait(mutex);
        if (!fib)
          fib = 1;
        else
          fib = fibNext(fib);
        if (*finished) {
          semPost(mutex);
          exit(0);
        }
        printf("Decreaser processes finished their turn %d", turn++);
        semPost(mutex);
        printf("\n");
      }
      semPost(lock);
      printf("\n");
    }

    exit(0);
  }

  waitpid(child_pid, &status, 0); //Waiting for processes

  printf("Master Process: Killing all children and terminating the program\n");

  shmdt(memory);
  shmctl(shmid, IPC_RMID, NULL);

  /*sem_unlink("lckfile");
  sem_unlink("semfile");
  sem_close(smutex);
  sem_close(slock);*/

  return 0;
}