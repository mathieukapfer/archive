/*
 * Chrono.h
 *
 *  Created on: 21 juin 2012
 *      Author: louis
 */

#ifndef CHRONO_H_
#define CHRONO_H_

#include <sys/time.h>

/*
 *
 */
class Chrono {
 public:
  Chrono();
  virtual ~Chrono();

  // Start the time measurement
  void start();

  // Terminate the time measurement
  // @return the number of micro sec elapsed since start call
  long stop();

  // @return the number of micro sec on the last measurement
  long uSecElapsed();

 private:
  struct timeval _start;
  bool _running;
  long _elapsed;
};

#endif /* CHRONO_H_ */
