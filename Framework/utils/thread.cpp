#include <pthread.h>
#include "thread.h"
#include "SemCollection.h"
#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h> // va_list
#include <syslog.h>

// cannot use TraceDebug, LogServerThread will start a thread...
//TRACEDEBUG_REGISTER("[KERNEL][THREAD]");

Thread::Thread(char *name) :
    _quit(false),
    _name(name),
    Arg_(NULL) {
}

Thread::Thread(string name) :
    _quit(false),
    _name(name),
    Arg_(NULL) {
}

Thread::~Thread() {
  // int ret = 0;

  // _logSyslog("ENTER %s:%s", __FUNCTION__, _name.c_str());

  _quit = true;

#if 0
  void *res;
  int s;
  ret = pthread_cancel(ThreadId_);

  s = pthread_join(ThreadId_, &res);

  if (s != 0)
    _logSyslog(" pthread_join error");

  if (res == PTHREAD_CANCELED)
    _logSyslog("main(): thread was canceled");
  else
    _logSyslog("main(): thread wasn't canceled (shouldn't happen!)");
#endif

  // _logSyslog("EXIT %s:%s return %d", __FUNCTION__, _name.c_str(), ret);

}

void Thread::Exit() {
  int ret = 0;
  _logSyslog("pthread_exit called with %s", _name.c_str());
  pthread_exit(&ret);

}

int Thread::Start(void * arg) {
  Arg(arg);
  return Start();
}

void Thread::_logSyslog(char *format, ...) {
  va_list ap;
  char str[200];

  va_start(ap, format);

  // format string
  vsnprintf(str, sizeof(str), format, ap);

  // write it to syslog
  syslog(LOG_NOTICE, "%d|evse:%s", (int) time(0), str);

  printf("<!-- %s -->\n", str);

  va_end(ap);
}

int Thread::Start(void) {

  //_logSyslog("Start thread %s", _name.c_str());


  int code = pthread_create(&ThreadId_, NULL, Thread::EntryPoint, this);

  if (code) {
    _logSyslog("FATAL! ERROR creating thread %s", _name.data());
    exit(1);
  }

#if 0
  if (pthread_join(ThreadId_, NULL)) {
    // wait for the thread to finish
    _logSyslog("FATAL! ERROR joining thread %s", _name);
    exit(1);
  }
#endif

  return code;
}

void Thread::Join() {
  void* res;
  pthread_join(ThreadId_, &res);
}

void Thread::Run(void * arg) {
  Setup();
  SemCollection::inst()->registerThread(_name.c_str());
  Execute(arg);
}

/*static */
void * Thread::EntryPoint(void * pthis) {
  Thread * pt = (Thread *) pthis;

  pt->Run(pt->Arg());
  return NULL;
}

void Thread::Setup() {
  // Do any setup here
}

void Thread::Execute(void* arg) {
  // Shoul never be called! Can happen if Thread::Start is called within the constructor (Start is virtual an not ready to be called...)
  _logSyslog("FATAL! Thread::Start (%s) called from a constructor? Exit!", _name.c_str());
  exit(-1);
}

void Thread::Kill() {

  if (pthread_kill(ThreadId_, SIGPIPE)) {
    _logSyslog("pthread_kill called with %s - OK", _name.c_str());
  } else {
    _logSyslog("pthread_kill called with %s - KO", _name.c_str());
  }

}
