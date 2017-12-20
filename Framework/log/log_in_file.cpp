#include <stdio.h>
#include <stdarg.h> /* Pour va_list */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define LOG(format) log format
#define MAX_NB_LINES 3

void log(char *format, ...) {
  static int logCounter = 0;
  static int fd = 0;

  va_list ap;
  char str[255];

  if (fd == 0) {
    printf("<!--Create log file-->\n");
    fd = open("log.txt", O_RDWR | O_APPEND | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
  }

  va_start(ap, format);

  vsnprintf(str, 255, format, ap);

  printf("<!--add: %s in file %d-->\n", str, fd);

  write(fd, str, strlen(str));

  if (logCounter++ > MAX_NB_LINES) {
    close(fd);
    system("mv log.txt log.txt.old");
    fd = 0;
    logCounter = 0;
  }

  va_end(ap);
}

int main() {
  int i = 0;

  while (1) {
    LOG(("toto %d %d", i, i));
    i++;
    sleep(1);
  }
}

