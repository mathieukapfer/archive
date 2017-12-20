#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void* Receiver(void * param) {

  fd_set readfds_spec;
  fd_set readfds;
  struct timeval timeout_spec = { 1, 500000 };
  struct timeval timeout = timeout_spec;
  int pipe = *(int *) param;

  // Check pipe number
  printf("Receiver - check pipe number: %d\n", pipe);

  // Add pipe in fds
  FD_ZERO(&readfds_spec);
  FD_SET(pipe, &readfds_spec);

  while (1) {
    int ret;
    static int count = 0;

    // Copy fds spec in fds (will be modified by 'select' call)
    readfds = readfds_spec;

    // Wait pipe OR timeout
    // NOTE : timeout = {0,0} as 'select' argument is used for polling
    //        timeout = NULL  as 'select' argument is used to block indefinitely 
    if ((timeout.tv_sec == 0 && timeout.tv_usec == 0)) {
      // avoid polling
      ret = select(pipe + 1, &readfds, NULL, NULL, NULL);  // no timeout => block indefinitely
    } else {
      ret = select(pipe + 1, &readfds, NULL, NULL, &timeout);  // timeout
    }

    // check ret value and timeout (updated with remaining time)
    printf("select: %d (timeout %d,%d s)\n", ret, (int) timeout.tv_sec, (int) timeout.tv_usec);

    // if all ok, read pipe content
    if (ret > 0 && FD_ISSET(pipe, &readfds)) {
      char c[16];
      int nb;
      memset(&c, 0, sizeof(c));

      while ((nb = read(pipe, c, sizeof(c))) > 0) {
        // beautiful the output
        printf("Receiver (%d): ", nb);
        for (int i = 0; ((i < sizeof(c)) && (i < nb)); i++)
          printf("%c", c[i]);
        printf("\n");

        // clean buffer
        memset(&c, 0, sizeof(c));
      }
    }

    // wait 0,1 s to avoid to take all CPU if select return immediatly
    usleep(100000);

    // play just 20 times is enough
    count++;
    if (count > 20) exit(0);
  }
}

int main() {
  int pipefd[2];
  char c[] = "hello";
  char c1[] = "init";
  pthread_t tid;
  int ret;

  ret = pipe(pipefd);
  if (ret == -1) {
    perror("pipe");
    // return -1;
    exit(0);
  }

  // Hum .. good to be sure of releasing pipe resources after execution
  fcntl(pipefd[0], F_SETFD, FD_CLOEXEC);
  fcntl(pipefd[1], F_SETFD, FD_CLOEXEC);

  // Needed to not block 'select' fct call (and actually use timeout parameter)
  fcntl(pipefd[0], F_SETFL, ::fcntl(pipefd[0], F_GETFL) | O_NONBLOCK);
  fcntl(pipefd[1], F_SETFL, ::fcntl(pipefd[1], F_GETFL) | O_NONBLOCK);

  printf("Main    - pipe: (%d,%d)\n", pipefd[0], pipefd[1]);

  // Filled pipe before launch consumer (for test)
  printf("write pipe[0]\n");
  write(pipefd[1], &c1, sizeof(c1));

  // Launch consumer
  if (pthread_create(&tid, NULL, Receiver, (void *) &pipefd[0])) {
    printf("\n ERROR creating thread 1");
    exit(1);

    if (pthread_join(tid, NULL)) {
      // wait for the thread 1 to finish
      printf("\n ERROR joining thread");
      exit(1);
    }
  }

  // Wait and filled pipe
  sleep(1);

  printf("write pipe[0]\n");
  write(pipefd[1], &c, sizeof(c));

  sleep(1);
  printf("write pipe[0]\n");
  write(pipefd[1], &c, sizeof(c));

  sleep(1);
  printf("write pipe[0]\n");
  write(pipefd[1], &c, sizeof(c));

  // wait end of joined thread
  pthread_exit(0);
}
