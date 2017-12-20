#ifndef DELAYSERVERPRIO_INCLUDED
#define DELAYSERVERPRIO_INCLUDED

#include "DelayServer.h"

class DelayServerPrio : public DelayServer {

 public:
  static DelayServerPrio * inst();
  static void deleteInst();

 private:

  // Singleton
  static DelayServerPrio * _globalPrio;
  static int _instanceNbPrio;

  DelayServerPrio();

};

#endif
