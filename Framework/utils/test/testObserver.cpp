/*
 * testObserver.cpp
 *
 *  Created on: 5 Aug 2015
 *      Author: Romain LE JEUNE (SESA381546)
 */

#include <cstdlib>
#include <cstdio>
#include <string>

#include "Observer.h"
#include "Observable.h"


class A : public Observable<A> {
 public:
  A() :
    Observable("A"),
    _innerName("") {
  }
  const std::string& getInnerName() const {
    return _innerName;
  }

  void setInnerName(const std::string& name) {
    _innerName = name;
    notify();
  }

 private:
  std::string _innerName;
};

class B : public Observer<A> {
 public:
  B() : Observer("B"){};
  virtual ~B(){};
  virtual void update(A *p_observable) {
    printf("==> name has changed to %s\n", p_observable->getInnerName().c_str());
  }
};

int main(int arc, char *argv[]) {
  A a;
  B b;
  a.addObserver(b);
  printf(" ------------------- INIT -----------------\n");
  a.setInnerName("toto");
  printf(" ------------------- titi -----------------\n");
  a.setInnerName("titi");
  printf(" ------------------- tutu -----------------\n");
  a.setInnerName("tutu");
  printf(" ------------------- tata -----------------\n");
  a.setInnerName("tata");
  return 0;
}

