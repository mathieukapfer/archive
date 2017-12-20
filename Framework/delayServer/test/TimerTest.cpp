#include "TimerInfoList.h"
#include "TimerEventI.h"
#include <iostream>
#include <unistd.h>

class MyTimerEvent : public TimerEventI {
 public:
  void TimerEvent(int timerid);
};

void MyTimerEvent::TimerEvent(int timerid) {
  cout << " TimerEvent: " << timerid << "\n";
}

void DisplayRemainingTime(TimerInfoList &list) {
  for (int i = 1; i < 6; ++i) {
    cout << "remaining time(" << i << "): " << list.getRemainingTime(i, NULL) << " ms\n";
  }
}

int main() {
  const bool isPrio = true;
  TimerInfoList list(isPrio);
  timeval tv;
  MyTimerEvent timerEvent;

  cout << "\nreg1 \n";
  list.registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);
  cout << "\nreg1+\n";
  list.registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);
  cout << "\nreg1+\n";
  list.registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);
  cout << "\nreg2 \n";
  list.registerTimer(2 /* id */, 2000 /* ms */, false, &timerEvent);
  cout << "\nreg3 \n";
  list.registerTimer(3 /* id */, 3000 /* ms */, false, &timerEvent);
  cout << "\nreg2+\n";
  list.registerTimer(2 /* id */, 2000 /* ms */, false, &timerEvent);
  cout << "\nreg7 \n";
  list.registerTimer(7 /* id */, 4000 /* ms */, false, &timerEvent);
  cout << "\nreg7+\n";
  list.registerTimer(7 /* id */, 4000 /* ms */, false, &timerEvent);
  cout << "\nreg4 \n";
  list.registerTimer(4 /* id */, 3500 /* ms */, false, &timerEvent);
  cout << "\nreg5 \n";
  list.registerTimer(5 /* id */, 3600 /* ms */, false, &timerEvent);
  cout << "\nreg6 \n";
  list.registerTimer(6 /* id */, 3700 /* ms */, false, &timerEvent);

  cout << "\nres \n";
  list.displayTimers();

  list.timerWait(tv);
  cout << "wait:" << tv.tv_sec << "," << tv.tv_usec << "\n";

  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";
  list.activateTimers();
  DisplayRemainingTime(list);

  sleep(1);
  cout << "sleep 1s\n";

  list.unregisterTimer(1 /* id */, &timerEvent);

  list.activateTimers();

  sleep(1);
  cout << "sleep 1s\n";

  cout << "test memory management \n";

  list.registerTimer(1 /* id */, 1000 /* ms */, true, &timerEvent);
  list.registerTimer(2 /* id */, 2000 /* ms */, false, &timerEvent);
  list.registerTimer(3 /* id */, 3000 /* ms */, false, &timerEvent);

}
