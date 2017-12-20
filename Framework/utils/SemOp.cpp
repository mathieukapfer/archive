#include <string.h>
#include <stdlib.h>

#include "SemOp.h"
#include "sem_helper.cpp"
#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[Sem][Op]");

SemOp::SemOp(int semTable, int semNumber) :
    _semTable(semTable),
    _semNumber(semNumber) {
  //LOG_ENTER("semTable:%d, semNumber:%d", semTable, semNumber);
}

const int SemOp::Lock() {
  int res;
  bool done = false;

  LOG_ENTER("%d,%d", _semTable, _semNumber);

  while (!done) {

    res = sem_lock(_semTable, _semNumber);

    if (res == -1) {
      if (errno != EINTR) {
        ///@todo(matk) add a specific message if evse is stopping, like in ModbusClient_::Close()
        LOG_WARNING("sem_lock failed for '%d:%d', %s (errno = %d)", _semTable, _semNumber, strerror(errno), errno);
        exit(0);
      }
    } else {
      done = true;
    }
  }

  return res;
}

const void SemOp::Unlock() {
  LOG_ENTER("%d,%d", _semTable, _semNumber);
  if ( sem_unlock(_semTable, _semNumber) ) {
    LOG_EXIT("%d,%d exit with error: %d", _semTable, _semNumber, errno);
  }
}
