#ifndef SEMTABLE_H
#define SEMTABLE_H

#include "SemOp.h"

class SemTable {
 public:

  static SemTable * GetInstance();
  virtual ~SemTable();

  void GetSem(int &semTable, int &semNumber);
  void ReleaseSem();

 private:

  SemTable();
  void Init();

  static SemTable *_global;

  // for private sem
  int _lastSemNumberUsed;
  int _semTable;
  int _semCounter;

  // mutex
  SemOp *_semOp;

};

#endif /* SEMTABLE_H */
