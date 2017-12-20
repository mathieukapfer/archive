// #include "helper.h"           /*  our own helper functions  */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "TcpServer.h"

#define LISTENQ        (1024)   /*  Backlog for listen()   */

#define NO_ERROR      (err >=0)

TcpServer::TcpServer(int port) :
    _port(port),
    _list_s(0) {
  printf("<!--Called %s-->\n", __FUNCTION__);
}

TcpServer::~TcpServer() {
  CloseConnection();
}

bool TcpServer::IsConnected() {
  return (_conn_s != 0);
}

/**
 * @brief create the socket server
 * @return error tag if triggered
 */
int TcpServer::CreateSocketServer() {
  printf("<!--Called %s-->\n", __FUNCTION__);
  /*  socket address structure  */
  struct sockaddr_in servaddr;
  int yes = 1;
  int err = 0;
  int ret = 0;

  /*  Create the listening socket  */
  if ((_list_s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fprintf(stderr, "<!--Error creating listening socket.-->\n");
    err = -1;
  }
  /*  Set socket option */
  if (NO_ERROR && setsockopt(_list_s, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes)) < 0) {
    fprintf(stderr, "<!--Error setting socket option.-->\n");
    close(_list_s);
    err = -1;
  }
  /*  Set all bytes in socket address structure to
   zero, and fill in the relevant data members   */
  if (NO_ERROR) {
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;  // htonl(INADDR_ANY);
    servaddr.sin_port = htons(_port);
  }
  /*  Bind our socket addresss to the 
   listening socket, and call listen()  */
  if (NO_ERROR && bind(_list_s, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
    fprintf(stderr, "<!--Error calling bind()-->\n");
    err = -1;
  }
  if (NO_ERROR && listen(_list_s, LISTENQ) < 0) {
    fprintf(stderr, "<!--Error calling listen()-->\n");
    err = -1;
  }
  if (NO_ERROR) {
    ret = _list_s;
  } else {
    ret = err;
  }
  return ret;
}

/**
 * @brief wait for connection
 * @return error statement if triggered
 */
int TcpServer::WaitConnection() {
  struct sockaddr_in addr;
  socklen_t addrlen = sizeof(addr);
  int ret = -1;
  /* Create server socket */
  if (!_list_s) {
    CreateSocketServer();
  }
  /*  Enter an infinite loop to respond
   to client requests and echo input  */
  // set it as HTML comment to not interfer with WWW
  printf("<!--Waiting client on port %d...-->\n", _port);

  bool done = false;
  while (!done) {
    /*  Wait for a connection, then accept() it  */
    if ((_conn_s = accept(_list_s, (struct sockaddr *) &addr, &addrlen)) < 0) {
      fprintf(stderr, "<!--Error calling accept() %s(%d)-->\n", strerror(errno), errno);
      _conn_s = 0;
      done = true;
    } else {
      printf("<!--The client %s is connected on port %d-->\n", inet_ntoa(addr.sin_addr), _port);
      ret = _conn_s;
      done = true;
    }
  }
  return ret;
}

/**
 * @brief close the connection
 * @return  error statement if triggered
 */
int TcpServer::CloseConnection() {

  int ret = 0;

  /*  Close the connected socket  */
  if (close(_conn_s) < 0) {
    fprintf(stderr, "<!--Error calling close()-->\n");
    ret = -1;
  }

  return ret;
}

