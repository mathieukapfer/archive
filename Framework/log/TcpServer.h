#ifndef TCPSERVER_INCLUDED
#define TCPSERVER_INCLUDED

// single client server 

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "TcpWrite.h"

class TcpServer : public TcpWrite {

 public:
  TcpServer(int port);
  virtual ~TcpServer();

  // wait the client
  int WaitConnection();

  // end the connection
  int CloseConnection();

  // client status connection
  bool IsConnected();

 protected:

  int CreateSocketServer();
  int _port;
  int _list_s; /* listening socket  */

};

#endif
