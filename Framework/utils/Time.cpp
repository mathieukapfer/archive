/*
 * Time.cpp
 *
 *  Created on: 25 juin 2012
 *      Author: bmo
 */

#include <cstring>
#include <iostream>
#include <cerrno>

extern "C" {
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
}

#include "Time.h"
#include "TraceDebug.h"
TRACEDEBUG_REGISTER("[WEB][TIME]")
using std::cout;

/* eclipse indexer doesn't find some time symbols
 * These are some dummy values that will NOT be COMPILED */
#ifndef CLOCK_REALTIME
#define CLOCK_REALTIME 0
#define gettimeofday(x, y) (x = y)
#define clockid_t int
#define usleep(x) sleep(x)
#endif

Time::Time() {
  m_timespec.tv_sec = m_timespec.tv_nsec = 0;
}

Time::~Time() {
}

/**
 * Overload of operator > of two Time object
 * @param   [in,out]    Time &t1    : first Time to compare
 * @param   [in,out]    Time &t2    : second Time to compare
 * @return true on success, false otherwise : bool ret
 */
bool operator>(Time &t1, Time &t2) {

  bool ret;

  if ((t1.m_timespec.tv_sec + t2.m_timespec.tv_nsec / 1000000000L)
      > (t2.m_timespec.tv_sec + t2.m_timespec.tv_nsec / 1000000000L)) {
    ret = true;
  } else if (t2.m_timespec.tv_sec > t1.m_timespec.tv_sec) {
    ret = false;
  } else {
    // égalité sur les secondes, regarder les millisecondes
    if (t1.m_timespec.tv_nsec > t2.m_timespec.tv_nsec) {
      ret = true;
    } else {
      ret = false;
    }
  }

  return ret;
}

/**
 * Overload of operator == of two Time object
 * @param   [in]    Time &t1    : first Time to compare
 * @param   [in]    Time &t2    : second Time to compare
 * @return true on success, false otherwise : bool ret
 */
bool operator==(Time &t1, Time &t2) {
  bool ret = false;

  if ((t1.m_timespec.tv_sec == t2.m_timespec.tv_sec)
      && (t1.m_timespec.tv_nsec == t2.m_timespec.tv_nsec)) {
    ret = true;
  }

  return ret;
}

/**
 * Overload of operator != of two Time object
 * @param   [in]    Time &t1    : first Time to compare
 * @param   [in]    Time &t2    : second Time to compare
 * @return true on success, false otherwise : bool ret
 */
bool operator!=(Time &t1, Time &t2) {
  bool ret = false;

  if ((t1.m_timespec.tv_sec != t2.m_timespec.tv_sec)
      || (t1.m_timespec.tv_nsec != t2.m_timespec.tv_nsec)) {
    ret = true;
  }

  return ret;
}

/**
 * Get the time since the last write on the database file (text mode)
 * @param   [in]    string filename
 * @return Time object with the elapsed time    : Time t
 */
Time Time::GetWriteTimestamp(string filename) {
  Time t;
  struct stat data;
  if (stat(filename.c_str(), &data) != 0) {
    LOG_ERROR(" %s : %s ", filename.c_str(), strerror(errno));
  } else {
    t.m_timespec.tv_sec = data.st_mtim.tv_sec;
    t.m_timespec.tv_nsec = data.st_mtim.tv_nsec;
  }

  return t;
}

/**
 * Get the time since the last read on the database file (text mode)
 * @param   [in]    string filename
 * @return Time object with the elapsed time    : Time t
 */
Time Time::GetReadTimestamp(string filename) {
  Time t;
  struct stat data;
  if (stat(filename.c_str(), &data) != 0) {
    cout << filename.c_str() << ": " << strerror(errno);
  } else {
    t.m_timespec.tv_sec = data.st_atim.tv_sec;
    t.m_timespec.tv_nsec = data.st_atim.tv_nsec;
  }

  return t;
}

