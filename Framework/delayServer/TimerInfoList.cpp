#include "TimerInfoList.h"
#include "core_time.h"
#include "stdio.h"
#include "TraceDebug.h"
#include "errno.h"
#include "TimerDb.h"
#include "JitterMonitor.h"

TRACEDEBUG_REGISTER("[DelayServer][LIST]")

#define START_JITTER_MONITOR if (_monitor) _monitor->ComputeJitterStart();
#define STOP_JITTER_MONITOR(timerName)  if (_monitor) _monitor->ComputeJitter(timerName);

/*
 * Internal functions for manipulating timer data structures.  The
 * timerBitVec array is used for keeping track of timer identifiers.
 */

timeval TimerInfoList::system_gettime() {
  timeval tv;
#if 0
  gettimeofday(&tv, 0);
#else
  timespec ts;
  if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts) == -1) {
    LOG_ERROR("%s", strerror(errno));
  } else {
    tv.tv_sec = ts.tv_sec;
    tv.tv_usec = ts.tv_nsec / 1000;
  }
#endif
  return tv;
}

TimerInfoList::TimerInfoList(bool isPrio) :
  _mutex("timerList"),
  _monitor(NULL) {
  LOG_ENTER_();

  if (isPrio) {
    _monitor = new JitterMonitor("mastertimerPrio", 10);
  } else {
    _monitor = new JitterMonitor("mastertimer", 500);
  }

  currentTime = system_gettime();

//     firstTimerInfo = currentTimerInfo = 0;
}

TimerInfoList::~TimerInfoList() {
  LOG_ENTER_();

  list<TimerInfo*>::iterator i;

  for (i = _list.begin(); i != _list.end(); i++) {
    delete *i;
  }

  for (i = _freeList.begin(); i != _freeList.end(); i++) {
    delete *i;
  }

  _list.clear();
  _freeList.clear();
}

timeval TimerInfoList::updateCurrentTime() {
  return (currentTime = system_gettime());
}

/*
 insert timer info into list
 !!! MUTEX SHOULD BE TAKEN BE THE CALLED !!!
 */

void TimerInfoList::timerInsert(TimerInfo *ti) {
  list<TimerInfo*>::reverse_iterator i;

  LOG_ENTER();

  // _mutex.Lock();

  for (i = _list.rbegin(); i != _list.rend(); i++) {
    const TimerInfo * const t = *i;
    if (!(ti->timeout < t->timeout)) break;
  }

  _list.insert((i++).base(), ti);

  // _mutex.Unlock();

  displayTimers(true);
}

/*
 Returns the time to wait for the next timer, or null if no timers
 are waiting.
 */
bool TimerInfoList::timerWait(timeval &tm) {
  bool ret = true;

  timeval currentTime = updateCurrentTime();

  // Find first waiting timer not already active
  TimerInfo *t = 0;

  _mutex.Lock(true);

  for (list<TimerInfo*>::const_iterator it = _list.begin(); it != _list.end(); ++it) {
    if (!(*it)->inTimerEvent) {
      t = *it;
      break;
    }
  }

  if (!t) {
    ret = false;
  } else if (currentTime < t->timeout) {
    // time to wait
    tm = t->timeout - currentTime;
  } else {
    // no time to wait
    tm.tv_sec = 0;
    tm.tv_usec = 0;
  }

  _mutex.Unlock();

  return ret;
}

TimerInfo * TimerInfoList::getFreeTimerInfo(bool callInternally) {
  TimerInfo *ret = 0;

  if (!callInternally) {
    _mutex.Lock(true);
  }

  if (!_freeList.empty()) {
    // is there free timer left ?
    LOG_DEBUG("Recycle one slot");

    // recycle it
    ret = *_freeList.begin();
    // remove it from free list
    _freeList.pop_front();
  } else {
    LOG_DEBUG("Create a new slot");

    // create a new one
    ret = new TimerInfo;
  }

  if (!callInternally) {
    _mutex.Unlock();
  }

  return ret;
}

void TimerInfoList::registerTimer(int timerId, int interval, bool isPeriodic, TimerEventI *object) {
  TimerInfo *t;

  _mutex.Lock(true);

  // to improve stability keep only one timer with same ID per object
  unregisterTimer(timerId, object, true);

  // get free slot
  t = getFreeTimerInfo(true);

  // fill it
  t->id = timerId;
  t->interval.tv_sec = interval / 1000;
  t->interval.tv_usec = (interval % 1000) * 1000;
  t->timeout = updateCurrentTime() + t->interval;
  t->isPeriodic = isPeriodic;
  t->obj = object;
  t->inTimerEvent = false;

  timerInsert(t);

  _mutex.Unlock();

}

bool TimerInfoList::unregisterTimer(int timerId, TimerEventI *object, bool callInternally) {
  bool ret = false;

  LOG_ENTER();

  if (!callInternally) {
    _mutex.Lock(true);
  }

  // set timer inactive
  for (list<TimerInfo*>::iterator i = _list.begin(); i != _list.end(); i++) {
    register TimerInfo *t = *i;

    if ((t->id == timerId) && (!object || (object && (t->obj == object)))) {
      _list.erase(i);
//             if (t == firstTimerInfo)
//               firstTimerInfo = 0;
//             if (t == currentTimerInfo)
//               currentTimerInfo = 0;

// add the 'TimerInfo' useless to free list

// marked as processed (for security)
// t->inTimerEvent = true;

      _freeList.push_front(t);

      ret = true;
      break;
    }
  }

  if (ret) {
    displayTimers(true);
  }

  if (!callInternally) {
    _mutex.Unlock();
  }

  // Smart log
  if (!ret && callInternally == false) {
    // id not found : trace it (if not called internally)
    LOG_DEBUG("Timer %d not found in unregisterTimer (may be already expired or deleted) ", timerId);
  }

  return ret;
}

/*
 Activate pending timers, returning how many where activated.
 */
int TimerInfoList::activateTimers() {
  int n_act = 0;
  timeval currentTime;
  int maxCount = 0;
  TimerInfo currentTimerInfo;
  TimerInfo *currentTimerInfoPtr = 0;

  _mutex.Lock(true);
  maxCount = _list.size();
  _mutex.Unlock();

  while (maxCount-- > 0) {
    currentTime = updateCurrentTime();

    // **************************************
    // start Critical section
    // **************************************
    _mutex.Lock(true);

    if (false == _list.empty()) {
      //if (_list.begin() != _list.end()) {
      currentTimerInfoPtr = *(_list.begin());
    } else {
      currentTimerInfoPtr = 0;
    }

    // initial check
    if (  // protection against null pointer
    (currentTimerInfoPtr == 0) ||
    // no timer has expired
        (currentTime < currentTimerInfoPtr->timeout) ||
        // mark as in processing (avoid recusive called)
        (currentTimerInfoPtr->inTimerEvent == true)) {
      // leave critical secion
      _mutex.Unlock();
      // leave the loop
      break;
    }

    // get a local copy
    currentTimerInfo = *currentTimerInfoPtr;

    // remove from list
    _list.pop_front();

    // treat timer type
    if (currentTimerInfoPtr->isPeriodic) {
      // treat peridic timer
      // determine next timeout time
      currentTimerInfoPtr->timeout += currentTimerInfoPtr->interval;

      // next time in the past, add it in the future to not lost periodicity
      if (currentTimerInfoPtr->timeout < currentTime) {
        currentTimerInfoPtr->timeout = currentTime + currentTimerInfoPtr->interval;
      }

      // reinsert timer
      timerInsert(currentTimerInfoPtr);
    } else {
      // clean one shot timer
      // add it to free list
      _freeList.push_front(currentTimerInfoPtr);
    }

    // marked as processed
    // currentTimerInfoPtr->inTimerEvent = true;

    // **************************************
    // leave critical secion
    // **************************************
    _mutex.Unlock();

    // Processing
    // -----------
    n_act++;

    LOG_DEBUG(" TimerEvent called - enter");
    {
      START_JITTER_MONITOR;
      currentTimerInfo.obj->TimerEvent(currentTimerInfo.id);
      STOP_JITTER_MONITOR(TimerDb::inst()->GetName(currentTimerInfo.id));
    }
    LOG_DEBUG(" TimerEvent called - exit");

  }  // end of while

  if (n_act) {
    displayTimers(true);
  }

  return n_act;
}

#if 0
int TimerInfoList::popTimers() {
  int nbTimer = 0;

  while (!_list.empty()) {
    TimerInfo *currentTimerInfo = *(_list.begin());

    LOG_INFO("pop timer %d", currentTimerInfo->id);

    currentTimerInfo->obj->TimerEvent(currentTimerInfo->id);

    _list.pop_front();

    nbTimer++;
  }

  return nbTimer;
}
#endif

bool TimerInfoList::isTimerActif(int timer, TimerEventI *obj) {
  list<TimerInfo*>::iterator i;
  bool ret = false;

  _mutex.Lock(true);

  for (i = _list.begin(); i != _list.end(); i++) {
    TimerInfo *currentTimerInfo = *i;
    if ((currentTimerInfo->id == timer) && ((obj == NULL) ||  // do not check object owner
        ((obj != NULL) && (currentTimerInfo->obj == obj))  // check object owner
    )) {
      ret = true;
    }
  }

  _mutex.Unlock();

  return ret;
}

void TimerInfoList::displayTimers(bool callInternally) {
  bool print = false;

  list<TimerInfo*>::iterator i;

  TEST_LOG_DEBUG {
    print = true;
  }

  if (!callInternally) {
    // force the log
    print = true;
  }

  if (print) {

    log("\n");
    log("timers list\n");
    log("-----------\n");

    if (!callInternally) {
      // _mutex.Lock(true);
    }

    // display timer list
    for (i = _list.begin(); i != _list.end(); i++) {
      log("  actif: id %d (%20s) - interval %d,%06d - timeout %d,%06d - periodic : %d\n", (*i)->id,
          TimerDb::inst()->GetName((*i)->id), (int) (*i)->interval.tv_sec,
          (int) (*i)->interval.tv_usec, (int) (*i)->timeout.tv_sec, (int) (*i)->timeout.tv_usec,
          (*i)->isPeriodic);
    }

    // display free list
    for (i = _freeList.begin(); i != _freeList.end(); i++) {
      log("  freed: id %d (%20s) - interval %d,%06d - timeout %d,%06d - periodic : %d\n", (*i)->id,
          TimerDb::inst()->GetName((*i)->id), (int) (*i)->interval.tv_sec,
          (int) (*i)->interval.tv_usec, (int) (*i)->timeout.tv_sec, (int) (*i)->timeout.tv_usec,
          (*i)->isPeriodic);
    }

    if (!callInternally) {
      // _mutex.Unlock();
    }

  }

}

int TimerInfoList::getRemainingTime(int timer, TimerEventI *obj) {
  list<TimerInfo*>::iterator i;
  timeval remaingTime;
  bool isFound = false;
  int ret = -1;

  _mutex.Lock(true);

  for (i = _list.begin(); i != _list.end(); i++) {
    TimerInfo *currentTimerInfo = *i;
    if ((currentTimerInfo->id == timer) && ((obj == NULL) ||  // do not check object owner
        ((obj != NULL) && (currentTimerInfo->obj == obj))  // check object owner
    )) {
      isFound = true;
      remaingTime = currentTimerInfo->timeout - system_gettime();
    }
  }

  _mutex.Unlock();

  if (isFound) {
    ret = remaingTime.tv_sec * 1000 + remaingTime.tv_usec % 1000;
  }

  return ret;
}
