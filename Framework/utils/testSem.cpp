#include "Sem.h"
#include "SemCollection.h"
#include "TraceDebug.h"
#include "stub.h"
//#define TEST_PUBLIC_SEM

int main() {

#if TEST_PUBLIC_SEM
  Sem sem("test", false);
  Sem sem2("test2", false);
#else
  Sem sem("test");
  Sem sem2("test2");
#endif

  SemCollection::inst()->registerThread("main");

  sem.Unlock();
  monitor(0);
  sem.Unlock();
  monitor(0);

  sem.Lock();
  monitor(0);
  sem.Lock();
  monitor(0);

  sem.Unlock();
  monitor(0);
  sem.Unlock();
  monitor(0);

  delete (DelayServer::inst());
  delete (DelayServerPrio::inst());
  delete (SemCollection::inst());
  delete (TraceDebug::inst());

}
