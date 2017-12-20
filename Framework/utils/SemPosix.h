#ifndef SEM_POSIX_INCLUDED
#define SEM_POSIX_INCLUDED

#include "SemI.h"

#include <semaphore.h>

class SemPosix {

 public:

  SemPosix(const char * name);

  void Unlock();
  void Lock();
  int Getvalue();

 private:

  void Create();

  sem_t _sem;
  const char * _name;

};

#endif
