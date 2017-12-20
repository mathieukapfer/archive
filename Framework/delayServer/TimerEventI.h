#ifndef TIMEREVENTI_INCLUDED
#define TIMEREVENTI_INCLUDED

class TimerEventI {
 public:
  virtual ~TimerEventI() {
  }

  // timer callback 
  virtual void TimerEvent(int timerid) = 0;

  // explicite timer api
  void StartTimer(int timerId, TimerEventI *obj, int duration = 0);
  void StopTimer(int timerId, TimerEventI *obj);

  // implicite timer api
  void StartTimer(int timerId, int duration = 0);
  void StopTimer(int timerId);

  // check timer state
  // 3 usages : 
  // 1 - isTimerActif(<timerId>)                                => check if the timer <timerId> is in progress for the current class
  // 2 - isTimerActif(<timerId>, false )                        => check if the timer <timerId> is in progress for any classes
  // 3 - isTimerActif(<timerId>, false, <another TimerEventI*> )=> check if the timer <timerId> is in progress for another class

  bool isTimerActif(int timer, bool forThisClassOnly = true, TimerEventI *obj = 0);

};

#endif
