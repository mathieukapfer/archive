#include <semaphore.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include "SemPosix.h"

#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[KERNEL][SEM_POSIX]");

#define INITIALE_VALUE 1

SemPosix::SemPosix(const char *name) {
  _name = name;
  Create();
}

void SemPosix::Create() {
  LOG_DEBUG("ENTER Create sem %s", _name);

  if (sem_init(&_sem, 0, INITIALE_VALUE) < 0) {
    LOG_FATAL("Create failed on %s, errno = %d", _name , errno);
    exit(0);
  }

}

void SemPosix::Lock() {
  LOG_DEBUG("ENTER Lock sem %s ", _name);
  Getvalue();

  if (sem_wait(&_sem) < 0) {
    LOG_FATAL("Getvalue failed on '%s', errno = %d", _name , errno);
  }

}

void SemPosix::Unlock() {
  LOG_DEBUG("ENTER Unlock sem %s", _name);

  if (sem_post(&_sem) < 0) {
    LOG_FATAL("Getvalue failed on '%s', errno = %d", _name , errno);
  }

  Getvalue();

}

int SemPosix::Getvalue() {
  int value;

  if (sem_getvalue(&_sem, &value) < 0) {
    LOG_FATAL("Getvalue failed on '%s', errno = %d", _name , errno);
  }

  LOG_DEBUG(" %s:%d ", _name, value);

  return value;
}
