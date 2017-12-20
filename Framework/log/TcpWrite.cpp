#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "TcpWrite.h"

/*  TcpServer::Write a line to a socket  */

  TcpWrite::TcpWrite() :
      _conn_s(0) {
  }


ssize_t TcpWrite::WriteStr(char *str) {

  size_t nleft = strlen(str);
  ssize_t nwritten = 0;
  char *buffer;
  int sockd;

  sockd = _conn_s;
  buffer = str;

  if (sockd == 0) {
    nwritten = -1;
  }

  while (nleft > 0 && nwritten >= 0) {
    if ((nwritten = send(sockd, buffer, nleft, MSG_NOSIGNAL)) <= 0) {
      printf("<!--Send error: %s(%d)-->\n", strerror(errno), errno);

      switch (errno) {
        case EINTR:
          nwritten = 0;
          break;
        case EPIPE:
          printf("<!--Client disconnection-->\n");
          _conn_s = 0;
          nwritten = -2;
          break;
        default:
          nwritten = -1;
          break;
      }

    } else {
      buffer += nwritten;
      nleft -= nwritten;
    }

  }

  return nwritten;
}
