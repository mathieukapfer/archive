#include "JitterMonitor.h"
#include "core_time.h"
#include "TraceDebug.h"
#include "errno.h"

TRACEDEBUG_REGISTER("[JITTER]");

JitterMonitor::JitterMonitor(const char * jitterName, int criticalTime) :
    _jitterName(jitterName),
    _criticalTime(criticalTime) {
  deltaMaxMax = 0;
  Reset();
}

void JitterMonitor::Reset() {
  memset(&lastTv, 0, sizeof(struct timeval));
  deltaMax = 0;
  sumOfTime = 0;
  nbDelta = 0;
}

// todo@mka: same as TimerInfoList::system_gettime()
timeval JitterMonitor::system_gettime() {
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

void JitterMonitor::ComputeJitterStart() {
  // set reference time as current time
  lastTv = system_gettime();
}

int JitterMonitor::ComputeJitterDelta() {
  // compute accurancy
  timeval tv;
  timeval deltaTv;
  int delta = 0;

  // take current time
  tv = system_gettime();

  // compute delta
  if (lastTv.tv_sec != 0) {
    // compute delta in ms
    deltaTv = (tv - lastTv);
    delta = (deltaTv.tv_sec * 1000) + (deltaTv.tv_usec) / 1000;
  }
  // save current time
  lastTv = tv;

  return delta;
}


void JitterMonitor::ComputeJitter(char* msg) {

  int delta = ComputeJitterDelta();

  if (delta > 0) {
    // compute delta max
    if (delta > deltaMax) {
      deltaMax = delta;

      // compute the utlimate max
      if (delta > deltaMaxMax) {
        deltaMaxMax = delta;
      }

      TEST_LOG_INFO {
        DisplayJitter();
      }
    }

    // compute mean timer
    sumOfTime += delta;
    nbDelta++;

    if (delta > _criticalTime) {
      LOG_WARNING("%s:%s critical value:%dms", _jitterName, msg?msg:"", delta);
    }

#if 1
    // periodic output (and reset)
    /* >= 1 hour (delta time >= 5 ms => 200 * delta >= 1s */
    if ((nbDelta % (200 /* * 60 * 60 */)) == 0) {
      TEST_LOG_DEBUG {
        DisplayJitter();
      }
    }
#endif
  }
}

void JitterMonitor::DisplayJitter() {
  LOG_ENTER();

  log("mean timer %s: %3dms [nb cycles:%10d], max timer: %3dms, maxmax: %3dms\n", _jitterName,
      nbDelta ? (int) (sumOfTime / nbDelta) : 0, nbDelta, deltaMax, deltaMaxMax);

  nbDelta = 0;
  sumOfTime = 0;
}

void JitterMonitor::ComputeAndLog(std::string p_log) {
  int delta = ComputeJitterDelta();

  if (delta > _criticalTime) {
    LOG_WARNING("Jitter %s: %3dms [%s]", _jitterName, delta, p_log.c_str());
  } else {
    LOG_INFO("Jitter %s: %3dms [%s]", _jitterName, delta, p_log.c_str());
  }
}
