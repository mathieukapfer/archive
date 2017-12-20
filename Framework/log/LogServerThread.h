#include "thread.h"
#include "Sem.h"
#include "TcpServer.h"

#include "TcpWrite.h"

#define DEFAULT_LOG_SERVER_PORT 1501

class LogServerThread : public Thread, public TcpWriteI {

 public:

  static LogServerThread* GetInstance(int port = 0);
  ~LogServerThread();

  bool IsConnected();

  virtual ssize_t WriteStr(char *str);

 private:

  // singleton pattern
  static LogServerThread *_global;
  LogServerThread(int &port);

  // inherited from Thread
  void Execute(void * thread_param);

  // agregat
  TcpServer _tcpServer;
  Sem _semWaitClient;
  int _port;  // keep for memory

};
