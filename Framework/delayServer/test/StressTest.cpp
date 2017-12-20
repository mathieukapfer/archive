#include "thread.h"
#include "iostream"
#include "DelayServer.h"
#include "TimerEventI.h"
#include "TraceDebug.h"
#include "TimerDb.h"
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */
#include <sstream>

#if 1
#define TIMER_VALUE_IN_MS 1
#define SLEEP_VALUE_IN_MS 5
#define THREAD_MAX 10
#else
#define TIMER_VALUE_IN_MS 100
#define SLEEP_VALUE_IN_MS 500
#define THREAD_MAX 1
#endif

TRACEDEBUG_REGISTER("[TEST]")

// int gettid_() {
//   int tid = syscall(SYS_gettid);
//   return tid;
// }

class MyTimer: public TimerEventI {
  static int counter;
  // timer callback
  virtual void TimerEvent(int timerid)  {
    //LOG_ENTER_("ok(%d)", counter++);
    printf(".");
    fflush(stdout);
  }
};

int MyTimer::counter = 0;

class MyThread: public Thread {

 public:
  // const
  explicit MyThread(string name):Thread(name) {};

  virtual void Execute(void* arg) {
    MyTimer *myTimer = (MyTimer *) arg;
    int tid = syscall(SYS_gettid);
    LOG_ENTER_("tid:%d", tid);

    while (1) {
      LOG_INFO("tid:%d", tid);
      printf("%c", 'A' + tid%26);
      fflush(stdout);
      myTimer->StopTimer(TIMER_ID_BootNotification_Heartbeat);
      myTimer->StartTimer(TIMER_ID_BootNotification_Heartbeat, TIMER_VALUE_IN_MS);
      usleep(SLEEP_VALUE_IN_MS * 1000);
    }
  }

};

void createThreads(int threadNumber, MyThread *tab[], void * arg) {
  LOG_ENTER_();
  for (int i = 0; i < threadNumber; i++) {
    LOG_INFO("Create thread %d", i);
    std::stringstream name;
    name << "Thread" << i;
    tab[i] = new MyThread(name.str());
    // tab[i]->Start(arg);
  }
}

void startThreads(int threadNumber, MyThread *tab[], void * arg) {
  LOG_ENTER_();
  for (int i = 0; i < threadNumber; i++) {
    tab[i]->Start(arg);
  }
}

int main(int argc, char *argv[]) {

  MyTimer mytimer;
  MyThread *threadTab[THREAD_MAX];

  DelayServer *delayServer = DelayServer::inst();

  // Add dummy timer in order to avoid a null list at start up (fix this pb!)
  // mytimer.StartTimer(1, 1000);

  delayServer->Start();

  sleep(1);

  createThreads(THREAD_MAX, threadTab, (void *) &mytimer);
  startThreads(THREAD_MAX, threadTab, (void *) &mytimer);

  while (1) {
    sleep(1);
  }

  return 0;
}
