#include "Uptime.h"
#include <stdio.h>
#include <stdlib.h>
#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[UTILS][TIME]")

int32_t Uptime::GetUptime() {
  FILE * uptimefile;
  char uptime_chr[50];
  uint64_t uptime = 0;
  int32_t ret = 0;

  if ((uptimefile = fopen("/proc/uptime", "r")) == NULL) {
    LOG_WARNING("HAL_GetUpTime unable to open /proc/uptime");
  } else {
    fgets(uptime_chr, 12, uptimefile);
    fclose(uptimefile);
    uptime = strtol(uptime_chr, NULL, 10);
    LOG_DEBUG("HAL_GetUpTime : %ld", uptime);
    ret = uptime;
  }

  return ret;

}
