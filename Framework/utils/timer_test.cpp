#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

static timer_t Clock;

#define timercmp(a, b, CMP)                      \
  (((a)->tv_sec == (b)->tv_sec) ?                \
    ((a)->tv_nsec CMP(b)->tv_nsec) :             \
    ((a)->tv_sec CMP(b)->tv_sec))

int main() {
  int ret;
  struct sigevent Info;
  struct itimerspec Data;
  struct itimerspec its;
  struct itimerspec its_;
  int Status;

  Info.sigev_notify = SIGEV_SIGNAL;
  Info.sigev_signo = SIGALRM;
  Info.sigev_value.sival_int = 0;

  ret = timer_create(/* Clock_Id */CLOCK_REALTIME, /*---> OPEN*/
                     /* Evp */&Info,
                     /* Timerid */&Clock);

  Data.it_interval.tv_sec = 1000000;
  Data.it_interval.tv_nsec = 0;
  Data.it_value.tv_sec = 1000000;
  Data.it_value.tv_nsec = 0;

  Status = timer_settime(Clock, 0, &Data,
  NULL);

  while (1) {
    //sleep(1);
    int i = 0;
    //for (i = 0;i<5;i++)
    {
      Status = timer_gettime(Clock, &its);
      printf("timer_gettime -%d-(%dsec, %9dnsec) - reverse order \n", i++, its.it_value.tv_sec,
             its.it_value.tv_nsec);
      if (timercmp(&its.it_value, &its_.it_value, >)) printf("KO !!!!!\n");

      memcpy(&its_, &its, sizeof(its));
    }

  }

}
