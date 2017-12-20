#include <stdlib.h>
#include "filter.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "TraceDebug.h"

Filter::Filter(int window) :
    _window(window),
    _window_index(0) {
  _buffer = (int*) calloc(_window, sizeof(int));
  memset(_buffer, 0, _window * sizeof(int));
}

void Filter::AddItem(int item) {
  if (_window_index >= _window) _window_index = 0;
  _buffer[_window_index++] = item;
}

bool Filter::GetValueEQ(int & item) {
  bool ret = true;

  for (int i = 0; i < _window; i++)
    if (_buffer[i] != _buffer[0]) {
      ret = false;
      break;
    } else {
      item = _buffer[0];
    }

  return ret;
}

int Filter::GetMoy() {
  double moy = 0;
  for (int i = 0; i < _window; i++)
    moy += _buffer[i];
  return (int) (moy / _window);
}

/* Try to define a neutrale value
  - 0 is a valid value
  - INT_MIN = -2^31 is too dangerous
  - use '-1'
*/
#define CLEARED_VALUE -1

void Filter::Clear() {
  _window_index = 0;
  for (int i = 0; i < _window; i++) {
    _buffer[i] = CLEARED_VALUE;
  }
}

int Filter::GetMoyOfNewVal(bool dump) {
  double sum = 0;
  int nb_val = 0;
  int moy = 0;

  for (int i = 0; i < _window; i++) {
    if (_buffer[i] != CLEARED_VALUE) {
      if (dump) {
       log("[%d]:%d ", i, _buffer[i]);
      }
      sum += _buffer[i];
      nb_val++;
    }
  }

  if (nb_val > 0) {
    moy = (int) (sum / nb_val);
  } else {
    // try to return the less disturbing value
    moy = 0;
  }

  if (dump) {
    log("mean=%d\n", moy);
  }

  return moy;
}


Filter::~Filter() {
  free(_buffer);
}
