#include <list>
#include "Sem.h"

void monitor(int a);

class SemCollection {

 public:

  static SemCollection * inst();
  static void deleteInst(void);
  ~SemCollection();

  void Monitor();

  void registerSem(Sem *sem, bool isPrivate = true);

  void registerThread(const char* name);

  // release common resource
  void CleanupPublicSem();

  // neested type
  typedef struct {
    const char* name;
    int tid;
    int scheduler;
    int prio;
  } ThreadId;

 private:

  // singleton
  SemCollection();
  static SemCollection * _globalInst;

  // agrerat
  std::list<Sem*> _semList;
  std::list<Sem> _semPublicList;
  std::list<ThreadId> _threadList;

};
