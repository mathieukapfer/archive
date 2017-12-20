#include "ThresholdWithDelay.h"
#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[THRES]")

ThresholdWithDelay::ThresholdWithDelay(char* name, int timerId, TimerEventI &parent):
  _isInitialized(false),
  _name(name),
  _timerId(timerId),
  _parent(parent),
  _threshold(0),
  _delay(0) {};

ThresholdWithDelay::~ThresholdWithDelay() {}

void ThresholdWithDelay::SetConf(int threshold, int delay) {
  _threshold = threshold;
  _delay = delay;
  _isInitialized = true;
}

bool ThresholdWithDelay::Check(int measure) {
  bool ret = false;
  if (_isInitialized) {
    ret = Check_(measure);
  }
  return ret;
}

bool ThresholdWithDelay::Check_(int measure) {
  bool ret = false;
  bool isDelayOn = _parent.isTimerActif(_timerId);

  if ( (_threshold > 0) && (measure > _threshold) ) {
    if (!isDelayOn) {
      LOG_NOTICE("%s: conso:%d > threshold:%d => start delay(%dms)",
                 _name, measure, _threshold, _delay);
      _parent.StartTimer(_timerId, _delay);
      ret = true;
    }
  } else {
    if (isDelayOn) {
      LOG_NOTICE("stop %s Delay", _name);
      _parent.StopTimer(_timerId);
    }
  }

  LOG_EXIT("%s measure:%d > threshold:%d is %s", _name, measure, _threshold, ret?"true":"false");

  return ret;
}

void ThresholdWithDelay::Init() {
  _parent.StopTimer(_timerId);
};
