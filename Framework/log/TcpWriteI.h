#ifndef TCPWRITEI_INCLUDED
#define TCPWRITEI_INCLUDED

#include <sys/types.h>

// interface to write string on TCP socket

class TcpWriteI {

 public:
  virtual ~TcpWriteI(){};
  virtual ssize_t WriteStr(char *str) = 0;

};

#endif
