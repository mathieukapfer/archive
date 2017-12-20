#ifndef DELAYSERVER_INCLUDED
#define  DELAYSERVER_INCLUDED
#include "TimerInfoList.h"
#include "TimerEventI.h"
#include <sys/types.h>
#include <pthread.h>
#include "thread.h"

#define REGISTER_TIMER(a) {DelayServer:inst()->registerTimer a}

class DelayServer : public Thread {

 public:

  // singleton
  static DelayServer * inst();
  static void deleteInst();
  virtual ~DelayServer();

  // add/remove a timer
  void registerTimer(int timerId, int interval /* ms */, bool isPeriodic, TimerEventI *obj);
  bool unregisterTimer(int timerId, TimerEventI *obj);

  // check if timer is registred
  bool isTimerActif(int timer, TimerEventI *obj);

  // get remaining time
  // return value in ms if time is active, else negative value 
  int getRemainingTime(int timer, TimerEventI *obj);

  // end the delay server loop
  void stop();

  // for debug purpose
  void displayTimers();
  int activateTimers();

 protected:

  // inherited from Thread
  void Execute(void*);
  void Setup();

  // do the work : wait and then activate expired timer
  bool processEvents();

  // Singleton
  static DelayServer * _global;
  static int _instanceNb;
  static bool keepRunning;

  DelayServer(char * threadName, bool highPrio = false);

  // init pipe
  void Init();

 private:

  void wakeUp();
  void GetThreadInfo();

  int doSelect(timeval *timeout);

  int _pipefd[2];
  fd_set _readfds_spec;

  bool _isHighPrio;
  TimerInfoList _timerList;

};

#endif
