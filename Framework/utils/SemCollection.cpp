#include "SemCollection.h"
#include <iostream>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

#include <syscall.h>
#include "TraceDebug.h"

// for debian support
#include <unistd.h>
#include <sys/syscall.h>   /* For SYS_xxx definitions */

// for monitoring on SIG_USR2 only
#include "DelayServer.h"
#include "DelayServerPrio.h"

TRACEDEBUG_REGISTER("[SEM][LIST]")

int gettid_() {
  return syscall(SYS_gettid);
}

SemCollection * SemCollection::_globalInst = NULL;

SemCollection::SemCollection() {
  // register signal handle
  signal(SIGUSR2, monitor);
}

void SemCollection::registerSem(Sem *sem, bool isPrivate) {
  _semList.push_back(sem);

  if (!isPrivate) {
    // keep a copy of instance to easy the cleanup
    _semPublicList.push_back(*sem);
  }

}

void SemCollection::registerThread(const char* name) {
  ThreadId threadId;
  sched_param param;

  threadId.name = name;
  threadId.tid = (int) gettid_();
  threadId.scheduler = sched_getscheduler(0);

  // get prio
  if (sched_getparam(0, &param)) {
    threadId.prio = -1;  // error
  } else {
    threadId.prio = param.sched_priority;
  }

  _threadList.push_back(threadId);
}

SemCollection::~SemCollection() {
  LOG_ENTER_();
  _semList.clear();
  _semPublicList.clear();
  _threadList.clear();
  LOG_EXIT_("void");
}

void SemCollection::deleteInst(void) {
  if (_globalInst) {
    delete (_globalInst);
    _globalInst = NULL;
  }

}

SemCollection * SemCollection::inst() {
  if (!_globalInst) _globalInst = new SemCollection;

  return _globalInst;

}

void monitor(int a) {
  SemCollection::inst()->Monitor();

#ifndef TESTU
  DelayServer::inst()->displayTimers();

  DelayServerPrio::inst()->displayTimers();
#endif

}

void SemCollection::Monitor() {
  // monitor thread
  // monitor semaphore
  log("Thread table \n ----------- \n");
  log(" name                      : tid   (scheduler/prio) \n");
  {
    std::list<ThreadId>::iterator i;

    for (i = _threadList.begin(); i != _threadList.end(); i++) {
      log(" %-25s : %8d (%d,%d)\n", (*i).name, (*i).tid, (*i).scheduler, (*i).prio);
    }
  }

  // monitor semaphore
  log("\nSem table \n ----------- \n");
  log(" name            : owner=(pid,tid) - last waiter     - type  \n");
  {
    std::list<Sem*>::iterator i;

    for (i = _semList.begin(); i != _semList.end(); i++) {
      (*i)->Monitor();
    }
  }

}

// release common resources
void SemCollection::CleanupPublicSem() {

  LOG_ENTER_();

  std::list<Sem>::iterator i;

  for (i = _semPublicList.begin(); i != _semPublicList.end(); i++) {
    i->CleanupPublicSem();
    // _semPublicList.erase(i);
  }

}
