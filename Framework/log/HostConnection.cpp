#include "HostConnection.h"
#include "LogServerThread.h"
#include "TcpClient.h"
#include "TraceDebug.h"
#include <stdio.h>

#define HOST_LOG_SERVER_IP_FILE "hostIp.txt"
#define LOG_DEFAULT_SERVER_PORT 1501

HostConnection::HostConnection() :
    _hostLogServer(NULL),
    _logServer(NULL),
    _isConnected(false) {
  InitializeHostConnection();
}

HostConnection::~HostConnection() {
  delete _hostLogServer;
}

/**
 * @brief parse the hostIp.txt file to get the host address
 * @return the host IP address
 */
char *HostConnection::getHostAddr() {
  FILE* fd;
  static char buf[200];
  char *str = buf;

  fd = fopen(HOST_LOG_SERVER_IP_FILE, "r");

  // file is present
  if (fd) {
    if (fgets(str, sizeof(buf), fd) == NULL) {
      // file empty
      str = NULL;
    }
    // else str contains the 1st line of file
  } else {
    str = NULL;
  }

  // close the file descriptor if needed
  if (fd != 0) fclose(fd);

  return str;
}

void HostConnection::InitializeHostConnection() {

  // try first connection to host server
  _isConnected = InitializeHostServerMode();

  if (!_isConnected) {
    // launch target log server, host will be the client later
    InitializeHostClientMode();
  }

}

bool HostConnection::IsConnected() {
  bool ret = (_isConnected || (_logServer && _logServer->IsConnected()));

  return ret;
}

bool HostConnection::InitializeHostServerMode() {

  char * hostAddr = NULL;
  bool ret = false;

  // check if host addr is defined
  hostAddr = getHostAddr();

  if (hostAddr) {

    // create client socket 
    _hostLogServer = new TcpClient(hostAddr, LOG_DEFAULT_SERVER_PORT);

    // Try connection
    // FIX ME : remove 
    // printf("<!-- === Try connection to host log server @%s ... -->\n",hostAddr);
    printf("<!-- === Try connection to host log server ... -->\n");
    ret = _hostLogServer->ConnectToServer();
  }

  return ret;
}

void HostConnection::InitializeHostClientMode() {

  // target is tcp server
  printf("<!-- === Start log server ... -->\n");
  _logServer = LogServerThread::GetInstance();  // create instance
  _logServer->Start();

  printf("<!-- === Log server started -->\n");
}

/**
 * @brief write str in logServer
 * @param str
 * @return 
 */
ssize_t HostConnection::WriteStr(char *str) {
  ssize_t ret = 0;

  if (_isConnected /*_hostLogServer && _hostLogServer->IsConnected()*/) {
    ret = _hostLogServer->WriteStr(str);
  } else if (_logServer && _logServer->IsConnected()) {
    ret = _logServer->WriteStr(str);
  }
  return ret;
}

