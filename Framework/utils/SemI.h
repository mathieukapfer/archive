#ifndef SEM_I_INCLUDED
#define SEM_I_INCLUDED

class SemI {

 public:
  virtual ~SemI() {
  }

  virtual const void Unlock(bool checkMutexOwer = false) = 0;
  virtual const int Lock(bool checkMutexOwer = false) = 0;

  virtual const char* GetName() {
    return "UNKNOWN";
  }

};

#endif
