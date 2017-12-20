#include "DelayServerPrio.h"
#include "TraceDebug.h"
#include <stdlib.h>

TRACEDEBUG_REGISTER("[DelayServer][Prio]")

DelayServerPrio * DelayServerPrio::_globalPrio = 0;
int DelayServerPrio::_instanceNbPrio = 0;

DelayServerPrio::DelayServerPrio() :
    DelayServer("DelayServerPrio", true) {
  LOG_ENTER_();
}

DelayServerPrio * DelayServerPrio::inst() {

  if (!_globalPrio) {

    if (++_instanceNbPrio > 1) {
      LOG_FATAL("more than one DelayServer instance");
      exit(1);
    }

    _globalPrio = new DelayServerPrio();
  }

  return _globalPrio;
}


void DelayServerPrio::deleteInst() {
  LOG_ENTER_();

  if (_globalPrio) {

    // free global
    delete _globalPrio;

    // reinit global
    _globalPrio = NULL;
    _instanceNbPrio = 0;
  }
}
