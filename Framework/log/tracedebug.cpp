#include "tracedebug.h"

char * getLogLevelString(int loglevel) {
  char * ret = "UNKNOWN";
  loglevel = MASK_LOG_LSB(loglevel);
  if (loglevel == AFF_LOG_LEVEL_DEBUG)   ret =  LOG_LEVEL_DEBUG;
  else if (loglevel == AFF_LOG_LEVEL_INFO)    ret =  LOG_LEVEL_INFO;
  else if (loglevel == AFF_LOG_LEVEL_NOTICE)  ret =  LOG_LEVEL_NOTICE;
  else if (loglevel == AFF_LOG_LEVEL_WARNING) ret =  LOG_LEVEL_WARNING;
  else if (loglevel == AFF_LOG_LEVEL_ERROR)   ret =  LOG_LEVEL_ERROR;
  else if (loglevel == AFF_LOG_LEVEL_FATAL)   ret =  LOG_LEVEL_FATAL;
  return ret;
}
