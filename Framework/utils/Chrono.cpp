/*
 * Chrono.h
 *
 *  Created on: 21 juin 2012
 *      Author: louis
 */

#include <stdlib.h>
#include "Chrono.h"

Chrono::Chrono() :
    _running(false),
    _elapsed(0) {
}

Chrono::~Chrono() {
}

void Chrono::start() {
  gettimeofday(&_start, NULL);
  _elapsed = 0;
  _running = true;
}

long Chrono::stop() {
  if (_running) {
    _running = false;
    struct timeval now;
    gettimeofday(&now, NULL);
    _elapsed = ((now.tv_sec - _start.tv_sec) * 1000000 + (now.tv_usec - _start.tv_usec));
  }
  return _elapsed;
}

long Chrono::uSecElapsed() {
  return _running ? 0 : _elapsed;
}

