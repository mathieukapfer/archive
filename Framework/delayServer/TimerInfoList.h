#ifndef TIMERINFOLIST_INCLUDED
#define TIMERINFOLIST_INCLUDED

#include <list>
#include <sys/time.h>
#include <iostream>
#include "TimerEventI.h"
#include "Sem.h"

using namespace std;

// foward declaration
class JitterMonitor;

// internal timer info
struct TimerInfo {
  int id;  // - timer identifier
  timeval interval;  // - timer interval
  timeval timeout;  // - when to sent event
  TimerEventI *obj;  // - object to receive event
  bool isPeriodic;  // - rearm the timer periodically - TODO
  bool inTimerEvent;  // - if true means event is not yet sent
};

class TimerInfoList {
  // state variables used by activateTimers()
  //   TimerInfo *firstTimerInfo, *currentTimerInfo; 

 public:
  TimerInfoList(bool isPrio);
  virtual ~TimerInfoList();

  void registerTimer(int timerId, int interval, bool isPeriodic, TimerEventI *object);
  bool unregisterTimer(int timerId, TimerEventI *object, bool callInternally = false);

  bool timerWait(timeval &);
  timeval updateCurrentTime();

  // launch timers
  int activateTimers();

  // for memory management
  TimerInfo * getFreeTimerInfo(bool callInternally = false);

  // get remaining time
  // return value in ms if time is found else negative value 
  int getRemainingTime(int timer, TimerEventI *obj);

  // for debug purpose
  void displayTimers(bool callInternally = false);
  bool isTimerActif(int timer, TimerEventI *obj);

 private:
  timeval system_gettime();
  void timerInsert(TimerInfo *);

  timeval currentTime;

  list<TimerInfo*> _list;
  list<TimerInfo*> _freeList;

  Sem _mutex;

  JitterMonitor *_monitor;
};
#endif
