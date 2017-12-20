// Choose which Sem to test
// - comment the line below for Sem test
// - keep it uncommented for SemPosix test
//
//#define TEST_SEM_POSIX

#include <pthread.h>
#ifdef TEST_SEM_POSIX
#include "SemPosix.h"
#else
#include "Sem.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHER 30

const int maxCount = 5;
int count = 0;
int threadNb = 0;

SemI *sem;

void * ThreadAdd(void * a) {
  int id;
  int processId = *(int *) a;

  // take an id
  sem->Lock();
  id = threadNb++;
  printf(" Thread %d/%d has been created\n", processId, id);
  sem->Unlock();

  // use it
  while (1) {
    sem->Lock();

    printf("  Thread %d/%d get the token\n", processId, id);

    sleep(1);

    sem->Unlock();

    if (count++ > maxCount) {
      printf("  Thread %d/%d has finished is work\n", processId, id);
      break;
    }

    // sleep(1);
  }

  return NULL;
}

int main(int argc, char * argv[]) {
  pthread_t tid[NTHER];
  int processNumber = 0;

  if (argc > 1) {
    processNumber = atoi(argv[1]);
  }

  if (processNumber == 0) {
    printf("\n *********************************** \n");
    printf("\n   Allow to initialize public sem \n");
    printf("\n *********************************** \n");

    // EVSE process is the owner of PUBLIC SEM
    // So, set the right to initialize public sem
    Sem::AllowPublicSemInitialisation();
  } else {
    printf("\n ----------------------------------- \n");
    printf("\n NOT Allow to initialize public sem \n");
    printf("\n ----------------------------------- \n");
  }

#ifdef TEST_SEM_POSIX
  sem = new Sem("global");
#else
  sem = new Sem("global", false);
#endif

  for (int i = 0; i < NTHER; i++)
    if (pthread_create(&tid[i], NULL, ThreadAdd, (void *) (&processNumber))) {
      printf("\n ERROR creating thread 1");
      exit(1);

      if (pthread_join(tid[i], NULL)) {
        /* wait for the thread 1 to finish */
        printf("\n ERROR joining thread");
        exit(1);
      }
    }

  pthread_exit(NULL);
}

