#ifndef HOSTCONNECTION_INCLUDED
#define HOSTCONNECTION_INCLUDED

#include "TcpWrite.h"

class TcpClient;
class LogServerThread;

class HostConnection : public TcpWriteI {

 public:

  // cons
  HostConnection();
  ~HostConnection();

  // host status connection
  bool IsConnected();

  // inherited from TcpWriteI
  virtual ssize_t WriteStr(char *str);

 private:

  void InitializeHostConnection();
  bool InitializeHostServerMode();
  void InitializeHostClientMode();

  // retreive the host address from file
  char *getHostAddr();

  // connection 
  TcpClient *_hostLogServer;  // the host is the server
  LogServerThread *_logServer;  // the target is the server

  // connection status
  bool _isConnected;

};

#endif
