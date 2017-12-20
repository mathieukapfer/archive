/*
 * Time.h
 *
 *  Created on: 25 juin 2012
 *      Author: bmo
 * This class is used to count the elapsed time between the export of badge 
 * text database
 */

#ifndef TIME_H_
#define TIME_H_

#include <string>

extern "C" {
#include <stdint.h>
}

using std::string;

class Time {
 public:
  Time();
  virtual ~Time();
  static Time GetTime();
  static Time GetWriteTimestamp(string filename);
  static Time GetReadTimestamp(string filename);
  friend bool operator>(Time &t1, Time &t2);
  friend bool operator==(Time &t1, Time &t2);
  friend bool operator!=(Time &t1, Time &t2);
  struct timespec m_timespec;

 private:
};

#endif /* TIME_H_ */
