#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>

#include "TcpServer.h"

static int fd = 0;

int save(char *s) {
  int ret;
#if LOG_IN_FILE
  ret = write(fd, s, strlen(s));
#else
  ret = send(fd, s, strlen(s), MSG_NOSIGNAL);
#endif

  return ret;

}

int main() {

#if LOG_IN_FILE
  if (!fd)
  {
    fd = open("log.txt", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    printf("open: %d\n",fd);
  }
#else
  TcpServer tcpServer(1501);
  fd = tcpServer.WaitConnection();

  // FIXME
  //fd << "ok";
#endif

  int count = 0;

  while (1) {
    sleep(1);
    int ret;

    char str[20];
    snprintf(str, 20, "hello %d", count++);

    ret = tcpServer.WriteStr("toto\n");
    // ret = save("toto\n");

    printf("ret %d\n", ret);
  }

}
