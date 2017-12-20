#include "DelayServerPrio.h"
#include "TimerEventI.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "TraceDebug.h"
#include <unistd.h>

TRACEDEBUG_REGISTER("[test]")

#define TEST_PRIO

class MyTimerEvent : public TimerEventI {
 public:
  void TimerEvent(int timerid);
};

void MyTimerEvent::TimerEvent(int timerid) {
  TRACEDEBUG_REGISTER("[cb  ]")

#if 0
  cout << " TimerEvent: " << timerid << "\n";
#else
  LOG_INFO("TimerEvent: %d",timerid);
#endif
}

int main() {
#ifdef TEST_PRIO
  DelayServer *delayServerInst = DelayServerPrio::inst();
#else
  DelayServer *delayServerInst = DelayServer::inst();
#endif
  MyTimerEvent timerEvent;

  delayServerInst->Start();

  LOG_INFO("Order should be @0s:10");

  delayServerInst->registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);
  delayServerInst->registerTimer(2 /* id */, 2000 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(3 /* id */, 3000 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(7 /* id */, 4000 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(4 /* id */, 3500 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(5 /* id */, 3600 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(6 /* id */, 3700 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(10 /* id */, 0/* ms */, false, &timerEvent);

  sleep(1);
  LOG_INFO(" ---- Order should be @1s:1 ---- ");
  sleep(1);
  LOG_INFO(" ---- Order should be @2s:1,2---- \n" );
  sleep(1);
  LOG_INFO(" ---- Order should be @3s:1,3,4,5,6 ---- ");
  sleep(1);
  LOG_INFO(" ---- Order should be @4s:1,7 ---- ");

  sleep(1);

  LOG_INFO("Stop period timer 1");
  delayServerInst->unregisterTimer(1, &timerEvent);

  sleep(2);

  LOG_INFO("Restart period timer 1");
  delayServerInst->registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);

  sleep(1);

  delayServerInst->registerTimer(10 /* id */, 1000 /* ms */, true, &timerEvent);
  delayServerInst->registerTimer(20 /* id */, 2000 /* ms */, false, &timerEvent);
  delayServerInst->registerTimer(30 /* id */, 3000 /* ms */, false, &timerEvent);

  sleep(3);

  LOG_INFO("Stop period timer 1,10");
  delayServerInst->unregisterTimer(1, &timerEvent);
  delayServerInst->unregisterTimer(10, &timerEvent);

  sleep(2);

}
