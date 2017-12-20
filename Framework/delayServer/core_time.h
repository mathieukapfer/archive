#include <time.h>

// Internal operator functions for timevals
inline timeval &normalizedTimeval(timeval &t) {
  while (t.tv_usec > 1000000l) {
    ++t.tv_sec;
    t.tv_usec -= 1000000l;
  }
  while (t.tv_usec < 0l) {
    --t.tv_sec;
    t.tv_usec += 1000000l;
  }
  return t;
}
inline bool operator<(const timeval &t1, const timeval &t2) {
  return t1.tv_sec < t2.tv_sec || (t1.tv_sec == t2.tv_sec && t1.tv_usec < t2.tv_usec);
}
inline bool operator==(const timeval &t1, const timeval &t2) {
  return t1.tv_sec == t2.tv_sec && t1.tv_usec == t2.tv_usec;
}
inline timeval &operator+=(timeval &t1, const timeval &t2) {
  t1.tv_sec += t2.tv_sec;
  t1.tv_usec += t2.tv_usec;
  return normalizedTimeval(t1);
}
inline timeval operator+(const timeval &t1, const timeval &t2) {
  timeval tmp;
  tmp.tv_sec = t1.tv_sec + t2.tv_sec;
  tmp.tv_usec = t1.tv_usec + t2.tv_usec;
  return normalizedTimeval(tmp);
}
inline timeval operator-(const timeval &t1, const timeval &t2) {
  timeval tmp;
  tmp.tv_sec = t1.tv_sec - (t2.tv_sec - 1);
  tmp.tv_usec = t1.tv_usec - (t2.tv_usec + 1000000);
  return normalizedTimeval(tmp);
}
inline timeval operator*(const timeval &t1, int mul) {
  timeval tmp;
  tmp.tv_sec = t1.tv_sec * mul;
  tmp.tv_usec = t1.tv_usec * mul;
  return normalizedTimeval(tmp);
}

