#ifndef SEM_INCLUDED
#define SEM_INCLUDED

#include "SemI.h"
#include "SemTable.h"
#include "SemOp.h"
#include <sys/types.h>

typedef enum {
  SEM_BEHAVOUR_MUTEX,
  // Typical use of MUTEX :
  // - protect integrity of variable
  // Special behaviour :
  // - allow the same thread to take the MUTEX several timer
  //   without be locked

  SEM_BEHAVOUR_TOKEN,
// Typical use of TOKEN
// - One thread give a token to another thread to launch a treatment
//   In this cas, the same thread take several time the same TOKEN
//   and should be locked on it each time
// Special behavour :
// - none

} SemBehavour;

class Sem : public SemI {

 public:

  Sem(const char * name, bool isPrivate = true, SemBehavour behavour = SEM_BEHAVOUR_MUTEX);
  ~Sem();

  const void Unlock(bool checkMutexOwer = false);
  const int Lock(bool checkMutexOwer = false);
  const char * GetName();

  // release common resource
  static void AllowPublicSemInitialisation();
  void CleanupPublicSem();

  // hack to control max value of 'token' semaphore
  const void ResetValue();

  void Monitor();

 private:

  void Create(bool internal);

  // for public MUTEX initialisation right
  static bool hasRightToInitializePublicMutex;

  // for associated file manipulation
  key_t File_To_Key();
  static bool _initialized;
  static const char * _baseName;
  void CreateDirectory();

  // core of semaphore
  int _semTable;
  int _semNumber;
  bool _isPrivate;
  const char * _name;
  SemBehavour _behavour;
  int _mutexCount;

  // semaphore facilites
  SemOp *_semOp;

  // for monitoring
  int _pid_owner;
  int _tid_owner;
  int _pid_waiter;
  int _tid_waiter;

};
#endif
