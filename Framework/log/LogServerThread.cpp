#include "LogServerThread.h"
#include "TraceDebug.h"
#include <stdio.h>

LogServerThread::~LogServerThread() {
  // leave the main loop
  Thread::Stop();
  _semWaitClient.Unlock();
  // join the main loop thread
  Thread::Join();
}

LogServerThread *LogServerThread::_global = 0;

LogServerThread* LogServerThread::GetInstance(int port) {
  printf("<!--Called %s with %d-->\n", __FUNCTION__, port);

  if (!_global) {

    // take default port ?
    if (port == 0) {
      port = DEFAULT_LOG_SERVER_PORT;
    }
    TraceDebug::disableLog();
    _global = new LogServerThread(port);
    TraceDebug::enableLog();

  }

  // dislay port used
  printf("<!--LogServerThread port:%d-->\n", _global->_port);

  return _global;

}

LogServerThread::LogServerThread(int &port) :
    Thread("LogServer"),
    _tcpServer(port),
    _semWaitClient("LogServer", true, SEM_BEHAVOUR_TOKEN),
    _port(port) {
  printf("<!--Called %s-->\n", __FUNCTION__);
}

ssize_t LogServerThread::WriteStr(char *str) {
  ssize_t ret = 0;

  if (_global && IsConnected()) {
    ret = _tcpServer.WriteStr(str);

    if (ret == -2) {
      // wait reconnection
      _semWaitClient.Unlock();
    }
  }

  return ret;
}

bool LogServerThread::IsConnected() {
  return _tcpServer.IsConnected();
}

void LogServerThread::Execute(void * thread_param) {
  while (1) {
    if (_semWaitClient.Lock() < 0 || _quit) {
      break;
    };

    _tcpServer.WaitConnection();
  }

  // exit properly the thread
  Exit();

  printf("<!--LogServerThread: END OF THREAD-->\n");
}

