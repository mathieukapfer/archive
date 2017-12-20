#ifndef SEMOP_H
#define SEMOP_H

class SemOp {

 public:

  SemOp(int semTable, int semNumber);
  virtual ~SemOp() {
  }

  const void Unlock();
  const int Lock();

 private:

  int _semTable;
  int _semNumber;

};

#endif /* SEMOP_H */
