#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "LogServerThread.h"

int main() {
  int ret;
  int count = 0;

  LogServerThread *logServer = LogServerThread::GetInstance();
  logServer->Start();

  while (1) {
    char str[20];
    snprintf(str, 20, "hello %d, ", count++);

    usleep(100000);

    // sdt output
    ret = write(0, str, strlen(str));
    //ret = send(0,str, strlen(str), 0);

    // tcp output
    ret = logServer->WriteStr(str);

  }

}
