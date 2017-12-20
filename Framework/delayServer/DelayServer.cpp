#include "DelayServer.h"
#include "TraceDebug.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include "SemCollection.h"

TRACEDEBUG_REGISTER("[DelayServer]")

bool DelayServer::keepRunning = false;

DelayServer * DelayServer::_global = 0;
int DelayServer::_instanceNb = 0;

DelayServer * DelayServer::inst() {

  if (!_global) {

    if (++_instanceNb > 1) {
      LOG_FATAL("more than one DelayServer instance");
      exit(1);
    }

    _global = new DelayServer("DelayServer");
  }

  return _global;
}

void DelayServer::deleteInst() {
  LOG_ENTER_();

  if (_global) {

    // free global
    delete _global;

    // reinit global
    _global = NULL;
    _instanceNb = 0;
  }
}

DelayServer::DelayServer(char * threadName, bool highPrio) :
    Thread(threadName),
    _isHighPrio(highPrio),
    _timerList(highPrio) {
  LOG_ENTER_("as '%s' name with %s", threadName, highPrio?"high prio":"normal prio");

  Init();
}

DelayServer::~DelayServer() {

  LOG_ENTER_();

  // Release thread if it was launched before
  if ( keepRunning ) {
    // leave the main loop
    stop();
    wakeUp();

    // join the main loop thread
    Thread::Join();
  }

}

void DelayServer::Init() {
  int ret;

  // Create pipe
  ret = pipe(_pipefd);

  if (ret == -1) {
    perror("pipe");
    // return -1;
    exit(0);
  }

  // Hum .. good to be sure of releasing pipe resources after execution
  fcntl(_pipefd[0], F_SETFD, FD_CLOEXEC);
  fcntl(_pipefd[1], F_SETFD, FD_CLOEXEC);

  // Needed to not block 'select' fct call (and actually use timeout parameter)
  fcntl(_pipefd[0], F_SETFL, ::fcntl(_pipefd[0], F_GETFL) | O_NONBLOCK);
  fcntl(_pipefd[1], F_SETFL, ::fcntl(_pipefd[1], F_GETFL) | O_NONBLOCK);

  // Add pipe in fds
  FD_ZERO(&_readfds_spec);
  FD_SET(_pipefd[0], &_readfds_spec);
}

void DelayServer::registerTimer(int timerId, int interval, bool isPeriodic, TimerEventI *obj) {
  // LOG_ENTER();
  LOG_DEBUG("registerTimer called : timer id=%d (is %speriodic)", timerId, (isPeriodic?"":"not "));
  _timerList.registerTimer(timerId, interval, isPeriodic, obj);

  // _timerList.displayTimers();
  // TODO(matk) - optimization : wakeUp only if new timer is the first in list

  wakeUp();
}

bool DelayServer::unregisterTimer(int timerId, TimerEventI *obj) {
  bool ret;

  // LOG_ENTER();
  LOG_DEBUG("unregisterTimer called : timer id=%d ", timerId);
  ret = _timerList.unregisterTimer(timerId, obj);

  return ret;

}

void DelayServer::wakeUp() {
  LOG_ENTER();
  char c = 0;
  /*safe_*/write(_pipefd[1], &c, 1);
  LOG_EXIT("");
}

void DelayServer::GetThreadInfo() {
  sched_param param;
  int sched = sched_getscheduler(0);

  if (sched_getparam(0, &param))
  LOG_ERROR("sched_getparam KO");

  LOG_INFO("sched: %d - sched_priority:%d", sched, param.sched_priority);
}

void DelayServer::Setup() {
  if (_isHighPrio) {
    struct sched_param param;
    int policy = SCHED_FIFO;

    LOG_INFO("Set high prio scheduler");

    // Define priority: max/2
    if (sched_getparam(0, &param) == 0) {
      param.sched_priority = (sched_get_priority_max(policy) >> 1);
    }

    // Set scheduler and priority
    if (sched_setscheduler(0, policy, &param) == 0) {
      LOG_INFO("setscheduler ok");
    } else {
      LOG_ERROR("error in setscheduler");
    }

  }

}

void DelayServer::Execute(void* arg) {

  LOG_INFO("DelayServerTask called");

  // Check scheduler and priority
  GetThreadInfo();

  keepRunning = true;

  while (keepRunning) {
    bool ret;

    ret = processEvents();

    LOG_DEBUG("DelayServer: process event status %d", ret);
  }

  LOG_EXIT_("bye bye");

  // exit properly the thread
  Exit();

}

bool DelayServer::processEvents() {
  bool ret = false;
  int nevents = 0;

  // return the maximum time we can wait for an event.
  // wait in indefinitely
  timeval *tm = 0;
  // return immediatly
  timeval wait_tm = { 0l, 0l };

  LOG_ENTER();

  if (_timerList.timerWait(wait_tm)) tm = &wait_tm;

  nevents = doSelect(tm);

  if (keepRunning) {
    // activate timer
    nevents += _timerList.activateTimers();
    // return true if we handled events, false otherwise
    ret = (nevents > 0);
  }
  return ret;
}

int DelayServer::doSelect(timeval *timeout) {

  int nsel;
  fd_set readfds;
  int pipe = _pipefd[0];

  LOG_ENTER();

  if (timeout) {
    LOG_DEBUG("timeout : %d,%d", (int) timeout->tv_sec, (int) timeout->tv_usec);
  } else {
    LOG_DEBUG("no timeout");
  }

  // needed in DelayServer::select()
  _timerList.updateCurrentTime();

  readfds = _readfds_spec;

  do {
    nsel = select(pipe + 1, &readfds, NULL, NULL, timeout);
    LOG_DEBUG("select return %d", nsel);
  } while (nsel == -1 && (errno == EINTR || errno == EAGAIN));

  // some other thread woke us up... consume the data on the thread pipe so that
  // select doesn't immediately return next time
  int nevents = 0;
  if (nsel > 0 && FD_ISSET(pipe, &readfds)) {
    char c[16];
    int ret;
    while ((ret = read(pipe, c, sizeof(c))) > 0) {
      LOG_DEBUG("read pipe (%d)", ret);
    }

    ++nevents;
  }

  return (nevents);
}

void DelayServer::displayTimers() {
  _timerList.displayTimers();
}

void DelayServer::stop() {
  keepRunning = false;
}

int DelayServer::activateTimers() {
  return (_timerList.activateTimers());
}

bool DelayServer::isTimerActif(int timer, TimerEventI *obj) {
  return (_timerList.isTimerActif(timer, obj));
}

int DelayServer::getRemainingTime(int timer, TimerEventI *obj) {
  return (_timerList.getRemainingTime(timer, obj));
}

