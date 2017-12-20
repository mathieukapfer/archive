#ifndef JITTERMONITOR_H
#define JITTERMONITOR_H

#include <sys/time.h>
#include <string>

// Jitter computation and display
class JitterMonitor {

 public:
  JitterMonitor(const char * jitterName, int criticalTimeInMs);
  virtual ~JitterMonitor() {
  }

  // check time between successives call of this method
  void ComputeJitter(char* msg = NULL);

  // set new reference time
  // as a consequence, the next call to 'ComputeJitter' will check the time
  // bewteen this function call and 'ComputeJitter' call
  void ComputeJitterStart();

  void ComputeAndLog(std::string p_log);

  void DisplayJitter();
  void Reset();

 private:
  timeval system_gettime();

  const char * _jitterName;
  int _criticalTime;

  struct timeval lastTv;
  int deltaMaxMax;
  int deltaMax;
  int nbDelta;
  long long sumOfTime;

  int ComputeJitterDelta();
};

#endif /* JITTERMONITOR_H */
