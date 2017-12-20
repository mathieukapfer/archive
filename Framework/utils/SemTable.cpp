#include <stdlib.h>

#include "SemTable.h"
#include "sem_helper.h"

#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[Sem][Table]")

#define INTERNAL_SEM_NUMBER 0

SemTable *SemTable::_global = NULL;

SemTable * SemTable::GetInstance() {
#if 0
  static SemTable *_global = new(SemTable);
#else
  if (_global == NULL) {
    _global = new (SemTable);
  }
#endif

  return _global;
}

SemTable::SemTable() :
    _lastSemNumberUsed(INTERNAL_SEM_NUMBER),
    _semTable(-1),
    _semCounter(0) {
  LOG_ENTER_();

  // Create table
  Init();

}

SemTable::~SemTable() {

  LOG_ENTER_();

  int res = 0;

  if (_semTable > 0) {
    res = sem_remove(_semTable);
  }

  if (res < 0) {
    LOG_ERROR("Private sem table delete error");
  }

  delete (_semOp);

  _global = NULL;

}

void SemTable::Init() {

  LOG_ENTER_();
  
  // Create sem table
  _semTable = sem_create_private_table();

  // Init value to 1 to be ready for mutex usage
  sem_init_value(_semTable, INTERNAL_SEM_NUMBER, 1);

  // Use the first semaphore of the set to protect itself
  _semOp = new SemOp(_semTable, INTERNAL_SEM_NUMBER);

}

void SemTable::GetSem(int &semTable, int &semNumber) {

  LOG_ENTER();

  semTable = _semTable;

  _semOp->Lock();
  semNumber = (++_lastSemNumberUsed);
  _semCounter++;

  if ( _lastSemNumberUsed >  PRIVATE_SEM_TABLE_SIZE ) {
    LOG_FATAL("\nFATAL ERROR - OUT OF BOUND - sem %d requested > %d maximum sem par set", _lastSemNumberUsed, PRIVATE_SEM_TABLE_SIZE);
    exit(0);
  }

  _semOp->Unlock();

  // LOG_EXIT_("semTable:%d, semNumber:%d",
  //            semTable, semNumber
  //            );
}

void SemTable::ReleaseSem() {

  LOG_ENTER_("semtable:%d, sem counter:%d", _semTable, _semCounter);

  if (--_semCounter <= 0) {
    LOG_INFO("remove sem table %d", _semTable);

    //res = sem_remove(_semTable);
    delete (this);

  }
}
