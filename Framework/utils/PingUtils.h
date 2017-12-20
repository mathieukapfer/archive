/* 
 * File:   PingUtils.h
 * Author: romain
 *
 * Created on 1 juillet 2014, 15:13
 */

#ifndef PINGUTILS_H
#define PINGUTILS_H

class PingUtils {
 public:
  PingUtils();
  virtual ~PingUtils();
  bool ping(char *AddrToPing);
 private:
};

#endif /* PINGUTILS_H */

