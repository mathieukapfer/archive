#ifndef TCPCLIENT_INCLUDED
#define TCPCLIENT_INCLUDED

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */
#include <unistd.h>           /*  misc. UNIX functions      */

#include "TcpWrite.h"

class TcpClient : public TcpWrite {

 public:

  TcpClient(char * serverAddrStr, uint16_t serverPort);

  // Try connection
  bool ConnectToServer();

  // Connection status
  bool IsConnected();

 private:

  // Connect to server
  int Tcp_Client_Socket(char * serverAddrStr, uint16_t serverPort);

  // IP string stuff
  bool Is_Internet_Address(char * hostName);
  in_addr_t Build_Internet_Address(char * mask);

  // server definition
  char * _serverAddrStr;
  uint16_t _serverPort;

};

#endif
