#ifndef TCPWRITE_INCLUDED
#define TCPWRITE_INCLUDED

#include <sys/types.h>

#include "TcpWriteI.h"

// interface to send log to host

class TcpWrite : public TcpWriteI {

 public:

  TcpWrite();
  virtual ~TcpWrite(){};

  virtual ssize_t WriteStr(char *str);

 protected:

  int _conn_s; /* connection socket */

};

#endif
