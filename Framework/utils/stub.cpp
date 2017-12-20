#include "stub.h"
#include <iostream>

DelayServer * DelayServer::inst() {
  static DelayServer *delayServer = new (DelayServer);
  return delayServer;
}

void DelayServer::displayTimers() {
  std::cout << "DelayServer is a stub\n";
}

DelayServerPrio * DelayServerPrio::inst() {
  static DelayServerPrio *delayServerPrio = new (DelayServerPrio);
  return delayServerPrio;
}

void DelayServerPrio::displayTimers() {
  std::cout << "DelayServerPrio is a stub\n";
}

