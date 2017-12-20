#ifndef LOGFORMATER_H
#define LOGFORMATER_H

#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/time.h>
#include <stdint.h>
#include <libgen.h>

// output descriptor and functions that use it

void log(char *format, ...);
void logSyslog(int level, char * LOG_LEVEL, char * file, int line, const char * tracedebug_tag, char *msg_prefix, char* msg_suffix, char *format, ...);
void log_(char *str);
timeval getTimestamp();
void logOCPP(char *format, ...);

#endif /* LOGFORMATER_H */
