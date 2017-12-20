#ifndef TCPSERVERMultiClients_INCLUDED
#define TCPSERVERMultiClients_INCLUDED

#include "TcpServer.h"

#define HEADER_SIZE 100
#define CLIENT_IP_STR_SIZE 16
#define MAX_CLIENT 50

class TcpServerMultiClients : public TcpServer {

 public:
  TcpServerMultiClients(int port);
  //~TcpServerMultiClients();

  // multi clients server interface :
  //  - dataFormater is called with for each data string received
  //    and a header with source ip is added
  int ReceivedFromMultipleClients(void (*dataFormater)(char * str));

 private:

  // formated output buffer
  char _header[HEADER_SIZE];
  char _lastHeader[HEADER_SIZE];

  // Ip table of clients 
  char _clientAddr[MAX_CLIENT][CLIENT_IP_STR_SIZE];

};

#endif
