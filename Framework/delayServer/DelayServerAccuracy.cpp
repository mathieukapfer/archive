#include "DelayServer.h"
#include "TimerEventI.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>

class MyTimerEvent : public TimerEventI {
 public:
  void TimerEvent(int timerid);
};

#define PERIODIC_TIMER_VALUE_IN_MS 10

static timeval lastTv = {0, 0};
static long deltaMax = 0;

void MyTimerEvent::TimerEvent(int timerid) {
  // compute accurancy
  timeval tv;
  long delta;

  // take current time
#if 0
  gettimeofday(&tv, 0);
#else
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  tv.tv_sec = ts.tv_sec;
  tv.tv_usec = ts.tv_nsec / 1000;
#endif

  // compute delta
  if (tv.tv_sec != 0) {
    delta = tv - lastTv;

    if (delta > deltaMax) {
      deltaMax = delta;
      printf("delta max: %ld us\n", deltaMax);
    }
  }

  // save current time
  lastTv = tv;

  // cout << " TimerEvent: " << timerid << "\n";
}

int main() {
  DelayServer *delayServerInst = DelayServer::inst();
  MyTimerEvent timerEvent;

  // launch periodic timer
  delayServerInst->registerTimer(1 /* id */, PERIODIC_TIMER_VALUE_IN_MS /* ms */, true,
                                 &timerEvent);

  delayServerInst->Join();

}
