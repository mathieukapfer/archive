#include "TimerDb.h"

// explicite timer api
void TimerEventI::StartTimer(int timerId, TimerEventI *obj, int duration) {
  TimerDb::inst()->StartTimer((TimerId) timerId, obj, duration);
}

void TimerEventI::StopTimer(int timerId, TimerEventI *obj) {
  TimerDb::inst()->StopTimer((TimerId) timerId, obj);
}

// implicite timer api
void TimerEventI::StartTimer(int timerId, int duration) {
  StartTimer((TimerId) timerId, this, duration);
}

void TimerEventI::StopTimer(int timerId) {
  StopTimer((TimerId) timerId, this);
}

bool TimerEventI::isTimerActif(int timer, bool forThisClassOnly, TimerEventI *obj) {
  return TimerDb::inst()->isTimerActif(timer, forThisClassOnly ? this : obj);
}
