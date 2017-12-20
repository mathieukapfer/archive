#include <sys/time.h>
#include <stdio.h>
#include <string.h>

#define timercmp(a, b, CMP)                   \
    (((a)->tv_sec == (b)->tv_sec) ?           \
     ((a)->tv_usec CMP(b)->tv_usec) :         \
     ((a)->tv_sec CMP(b)->tv_sec))

int main() {
  struct itimerval itv, itv_;
  struct timeval ivmax = { 100, 0 };
  struct itimerval itvmax = { { 100, 0 }, { 100, 0 } };

  // DOC:
  //   struct itimerval {
  //     struct timeval it_interval; /* next value */
  //     struct timeval it_value;    /* current value */
  //   };
  //   struct timeval {
  //         long tv_sec;                /* seconds */
  //         long tv_usec;               /* microseconds */
  //     };

  itv = itvmax;

  setitimer(ITIMER_REAL, &itv, NULL);

  while (1) {
    //sleep(1);
    int i = 0;
    //for (i = 0;i<5;i++)
    {
      getitimer(ITIMER_REAL, &itv);
      printf("timer_gettime -%d-(%dsec, %6dusec) - reverse order \n", i++, itv.it_value.tv_sec, itv.it_value.tv_usec);
      if (timercmp(&itv.it_value, &itv_.it_value, >)) printf("KO !!!!!\n");

      memcpy(&itv_, &itv, sizeof(itv));
    }

  }

}
