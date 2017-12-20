/**
 * Observer.h
 * Generic Observer object
 * Allowed to observe on observable object
 *  Created on: 4 Aug 2015
 *      Author: Romain LE JEUNE (SESA381546)
 */

#ifndef SRC_UTILS_OBSERVER_H_
#define SRC_UTILS_OBSERVER_H_
#include <string>
template <class K>
class Observable;

template <class T>
class Observer {
 public:
  explicit Observer(std::string p_name):_name(p_name){};
  virtual ~Observer(){};
  /**
   * call by the observable object to which we are registered
   * to notify a change
   * @param [in] p_observable  observable object which have changed
   */
  virtual void update(T *p_observable)= 0;

  /**
   * used for log - really usefull in debug
   */
  const std::string& getName() const {
    return _name;
  }

 private :
  std::string _name;
};

#endif /* SRC_UTILS_OBSERVER_H_ */
