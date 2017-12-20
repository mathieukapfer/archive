/*
 * This class alert the *parent* by a timer callback with *timerId* if
 * the *mesure* is upper than the *thresold* since more than the *delay*
 *   - timerId and parent is provided in constructor
 *   - threshold and delay is provided by SetConf()
 *   - the mesure is inject by Check()
*/
#include "TimerEventI.h"

class ThresholdWithDelay {
 public:

  explicit ThresholdWithDelay(char* name, int timerId, TimerEventI &parent);

  virtual ~ThresholdWithDelay();

  void SetConf(int threshold, int delay);
  bool Check(int measure);
  void Init();

 private:
  bool Check_(int measure);

  bool _isInitialized;
  char *_name;
  int _timerId;
  TimerEventI &_parent;

  int _threshold;
  int _delay;

};
