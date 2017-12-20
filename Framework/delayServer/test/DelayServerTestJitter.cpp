#include "DelayServer.h"
#include "TimerEventI.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// thread prio management
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

#include <time.h>
#include "core_time.h"

// #define SCHED_OTHER  0
// #define SCHED_FIFO  1
// #define SCHED_RR  2
// #define SCHED_BATCH  3

static long PERIODIC_TIMER_VALUE_IN_US = 10000;

class MyTimerEvent : public TimerEventI {
 public:
  void TimerEvent(int timerid);
};

void MyTimerEvent::TimerEvent(int timerid) {

  static timeval lastTv = { 0, 0 };
  static long deltaMaxMax = 0;
  static long deltaMax = 0;
  static long sumOfTime = 0;
  static long nbDelta = 0;

  // compute accurancy
  timeval tv, res;
  long delta = 0;

  // take current time
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  tv.tv_sec = ts.tv_sec;
  tv.tv_usec = ts.tv_nsec / 1000;

  // compute delta
  if (lastTv.tv_sec != 0 && lastTv.tv_usec != 0) {
    // compute delta
    res = tv - lastTv;
    res = normalizedTimeval(res);
    delta = res.tv_sec * 1000000 + res.tv_usec;

    //
    if ( delta < 0 ) {
      printf("!-: %ld,%ld - %ld,%ld = %ld,%ld\n",
             tv.tv_sec, tv.tv_usec,
             lastTv.tv_sec, lastTv.tv_usec,
             res.tv_sec, res.tv_usec
        );
    }

    //compute delta max
    if (delta > deltaMax) {
      deltaMax = delta;
      printf("max jitter: %ld us\n", deltaMax - PERIODIC_TIMER_VALUE_IN_US);

      // compute the utlimate max
      if (delta > deltaMaxMax) deltaMaxMax = delta;
    }

    // compute mean timer
    sumOfTime += delta;
    nbDelta++;

    // periodic output (and reset)
    if (nbDelta % 1000 == 0) {
      printf("mean timer: %ld us (max jitter: %ld - maxmax: %ld)\n", sumOfTime / nbDelta,
             deltaMax - PERIODIC_TIMER_VALUE_IN_US, deltaMaxMax - PERIODIC_TIMER_VALUE_IN_US);
      nbDelta = 0;
      sumOfTime = 0;
      deltaMax = 0;
    }
  }

  // save current time
  lastTv = tv;

  // cout << " TimerEvent: " << timerid << "\n";
}

// #define SCHED_OTHER  0
// #define SCHED_FIFO  1
// #define SCHED_RR  2
// #define SCHED_BATCH  3

int getthreadinfo(struct sched_param *param) {
  if (sched_getparam(0, param)) printf("error\n");
  int sched = sched_getscheduler(0);
  printf("sched: %d - sched_priority:%d\n", sched, param->sched_priority);
  return sched;
}

int main(int argc, char **argv) {
  DelayServer *delayServerInst;
  MyTimerEvent timerEvent;

  // managed prio
  int max, min;
  struct sched_param param;

  // Manage user parameters
  if (argc <= 1) {
    printf("Usage %s <timer duration in ms>\n", argv[0]);
    exit(0);
  } else {
    PERIODIC_TIMER_VALUE_IN_US = atoi(argv[1]) * 1000;
    printf("Launch test with timer %d ms\n", atoi(argv[1]));
  };

  // check prio value / policy
  printf("*** Prio supported for each policy *** \n");
  for (int policy = 0; policy < 3; policy++) {
    max = sched_get_priority_max(policy);
    min = sched_get_priority_min(policy);
    printf("policy %d : [%d-%d]\n", policy, min, max);
  }

  // set prio for delay server
  printf("*** Set prio and policy for delay server *** \n");
  getthreadinfo(&param);
  param.sched_priority = 50;
  if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
    printf("error in setscheduler\n");
  } else {
    printf("setscheduler ok\n");
  }
  getthreadinfo(&param);

  // create Delay server
  printf("*** Start delay server *** \n");
  delayServerInst = DelayServer::inst();

  // set prio for timer call back fct
  printf("*** Set prio and policy for delay server *** \n");
  getthreadinfo(&param);
  param.sched_priority = 99;
  if (sched_setscheduler(0, SCHED_FIFO, &param) != 0) {
    printf("error in setscheduler\n");
  } else {
    printf("setscheduler ok\n");
  }
  getthreadinfo(&param);

  // launch periodic timer
  delayServerInst->registerTimer(1 /* id */, PERIODIC_TIMER_VALUE_IN_US / 1000 /* ms */, true,
                                 &timerEvent);

  // enable delay server
  delayServerInst->Start();

  while (1) {
    sleep(1);
  }

  delayServerInst->Join();

}
