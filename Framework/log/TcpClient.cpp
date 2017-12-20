#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/tcp.h>

#include "TcpClient.h"

TcpClient::TcpClient(char * serverAddrStr, uint16_t serverPort) :
    _serverAddrStr(serverAddrStr),
    _serverPort(serverPort) {
}

bool TcpClient::ConnectToServer() {
  _conn_s = Tcp_Client_Socket(_serverAddrStr, _serverPort);

  if (_conn_s > 0) {
    printf("<!-- Connection to host log server done ! -->\n");
  } else {
    printf("<!-- Connection to host KO ! -->\n");
  }

  return (_conn_s > 0);
}

bool TcpClient::IsConnected() {
  return (_conn_s > 0);
}

bool TcpClient::Is_Internet_Address(char * hostName) {
  bool ret = false;

  if ((hostName[0] >= '0') & (hostName[0] <= '9')) {
    ret = true;
  } else {
    ret = false;
  }

  return ret;

} /* end Is_Internet_Address */

in_addr_t TcpClient::Build_Internet_Address(char * mask) {
  return inet_addr(mask);
} /* end Build_Internet_Address */

int TcpClient::Tcp_Client_Socket(char * serverAddrStr, uint16_t serverPort) {
  struct sockaddr_in serverAddr;
  int Status;
  int Fd;
  int Tmp = 1;

  if (Is_Internet_Address(serverAddrStr)) {
    serverAddr.sin_addr.s_addr = Build_Internet_Address(serverAddrStr);
  } else {
#if 0
    struct hostent * hostEntry;
    hostEntry = gethostbyname(serverAddrStr);
    if (hostEntry == 0) {
      printf("<!--ERROR : could not find IP adress of %s [please add it to /etc/hosts] -->\n", serverAddrStr);
      exit(-1);
    } else {
      serverAddr.sin_addr.s_addr = **(in_addr_t **)(hostEntry->h_addr_list);
    }
#else
    printf("<!--ERROR : could not find IP adress of %s [please add it to /etc/hosts] -->\n",
           serverAddrStr);
#endif
  }

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(serverPort);

  Fd = socket(/* Family */AF_INET,
  /* Kind */SOCK_STREAM,
              /* Protocol */IPPROTO_IP);

  Status = setsockopt(/* Socket */Fd,
  /* Level */IPPROTO_TCP,
                      /* Option */TCP_NODELAY,
                      /* Value */(void *) &Tmp,
                      /* Length */4);

  Status = connect(/* Fd */Fd,
  /* Adr */(struct sockaddr *) &serverAddr,
                   /* Len */sizeof(struct sockaddr_in));

  if (Status == 0) {
    /* Set the socket non blocking */
    Status = 1;
    Status = ioctl(Fd, FIONBIO, &Status);
  } else {
    // cleanup
    close(Fd);
    Fd = -1;
  }

  return Fd;
}
