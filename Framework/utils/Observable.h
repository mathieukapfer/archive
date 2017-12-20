/**
 * Observable.h
 * generic Observable object with a list of observer and the ability to notify them
 *  Created on: 4 Aug 2015
 *      Author: Romain LE JEUNE (SESA381546)
 */

#ifndef SRC_UTILS_OBSERVABLE_H_
#define SRC_UTILS_OBSERVABLE_H_
#include "vector"
#include <iterator>
#include "Sem.h"
#include "Observer.h"
#include "TraceDebug.h"

template<class T>
class Observable {
 public:
  explicit Observable(std::string p_name):_name(p_name), _sem(p_name.c_str()) {
  }

  virtual ~Observable() {
  }

  /**
   * Add observer to our observerList
   * @param [in] p_observer observer to add to our obervers list
   */
  void addObserver(Observer<T> &p_observer) {
    TRACEDEBUG_REGISTER("[UTILS][OBS]");
    LOG_INFO("ADD OBSERVER %s to %s", p_observer.getName().c_str(),  getName().c_str());
    _sem.Lock();
    _observers.push_back(&p_observer);
    _sem.Unlock();
    LOG_EXIT("%d", _observers.size());
  }

  /**
   * notify observer list that we have changed
   */
  void notify() {
    TRACEDEBUG_REGISTER("[UTILS][OBS]");
    LOG_ENTER("%d observers of %s \n", _observers.size(),  getName().c_str());
    typename std::vector<Observer<T> *>::iterator it;
    _sem.Lock();
    for (it = _observers.begin(); it != _observers.end(); it++) {
      LOG_DEBUG("notify %s ", (*it)->getName().c_str());
      (*it)->update(static_cast<T *>(this));
    }
    _sem.Unlock();
  }

  /**
   * used for log - really usefull in debug
   */
  const std::string& getName() const {
    return _name;
  }

 private :
  std::string _name;
  Sem _sem;
  std::vector<Observer<T> *> _observers;

};

#endif /* SRC_UTILS_OBSERVABLE_H_ */
