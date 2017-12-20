#include "SemPosix.h"
#include <stdio.h>

int main() {
  SemPosix sem("test");

  sem.Lock();

  sem.Unlock();
  sem.Unlock();
  sem.Lock();
  sem.Lock();

  printf("Will wait infinitly\n");
  sem.Lock();
  printf("you should never see that !\n");
}
