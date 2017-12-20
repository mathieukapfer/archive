#include <signal.h>
#include <stdarg.h> /* pour va_list */
#include <unistd.h> /* pour write */
#include <syscall.h>

#include "LogFormater.h"
#include "TraceDebug.h"
#include "HostConnection.h"

#ifdef  ENABLE_SYS_LOG
#include <syslog.h>
#endif

// String format
// ==============
#define USER_MSG_SIZE  255
#define PREFIX_SIZE     71
#define FILE_NAME_AND_LINE_SIZE 40

static int gettid() {
  int tid = syscall(SYS_gettid);
  return tid;
}

void log(char *format, ...) {
  va_list ap;
  char str[USER_MSG_SIZE];

  va_start(ap, format);

  // format string
  vsnprintf(str, sizeof(str), format, ap);
  // write it to log output
  log_(str);

  va_end(ap);
}

// private data to filter identical syslog messages
struct MessageSyslog {
  timeval _ts;
  std::string _message;
  int _nbFound;
  int _nbDropped;
};
#define NB_LOGS 20
int _lastLog = -1;
MessageSyslog _logs[NB_LOGS];
int _nbDroppedMessages = 0;

void logSyslog(int level, char * LOG_LEVEL, char * file, int line, const char * tracedebug_tag, char *msg_prefix, char* msg_suffix, char *format, ...) {
  char prefix[PREFIX_SIZE];
  char file_name_and_line[FILE_NAME_AND_LINE_SIZE];

  timeval ts = getTimestamp();

  std::string filename = basename(file);
  size_t lastindex = filename.find_last_of(".");
  if (lastindex > 0) {
    filename = filename.substr(0, lastindex);
  }

  // format prefix
  snprintf(prefix,
           PREFIX_SIZE,
           "%04d%s%-26s%04u.%04u [%-5s]%-22s", gettid(), TraceDebug::getMasterSlavePrefix().c_str(), (snprintf(file_name_and_line, FILE_NAME_AND_LINE_SIZE, "%.21s:%d ", filename.c_str(), line), file_name_and_line), (uint32_t) ts.tv_sec, (uint32_t) ts.tv_usec / 100, (char *) LOG_LEVEL, tracedebug_tag);

  va_list ap;
  char message[USER_MSG_SIZE];

  va_start(ap, format);
  // format string
  vsnprintf(message, sizeof(message), format, ap);
  va_end(ap);

  char fulllog[PREFIX_SIZE+USER_MSG_SIZE];
  snprintf(fulllog, sizeof(fulllog), "%s%s%s%s", prefix, msg_prefix, message, msg_suffix);
  log_(fulllog);

  if (level <= AFF_LOG_LEVEL_INFO) {
    // FORCE|FATAL|ERROR|WARNING|NOTICE|INFO but not DEBUG
    bool sendSyslog = true;

    if (level <= AFF_LOG_LEVEL_WARNING) {
      //FORCE|FATAL|ERROR|WARNING but not NOTICE|INFO|DEBUG

      // filter messages below warning to display only one message over 10 (starting after 5 messages) in the last 300s

      int nbFound = 0;
      int nbDropped = -1;
      for (int i = 0; i < NB_LOGS && nbFound == 0; i++) {
        if (_lastLog >= i) {
          if (_logs[(_lastLog-i)%NB_LOGS]._message.compare(std::string(file_name_and_line) + std::string(message)) == 0) {
            // identical message: same message + file + line
            if (ts.tv_sec - _logs[(_lastLog-i)%NB_LOGS]._ts.tv_sec < 300) {
              // only if last message is less than 300s old
              nbFound = _logs[(_lastLog-i)%NB_LOGS]._nbFound;
              nbDropped = _logs[(_lastLog-i)%NB_LOGS]._nbDropped;
            }
          }
        }
      }
      nbFound++;
      if (nbFound >= 5 && nbDropped % 10 != 9) {
        //log("%d New message %s (%d.%d) found %d times (%d), drop it\n", _lastLog, message, ts.tv_sec, ts.tv_usec, nbFound, nbDropped);
        sendSyslog = false;
        _nbDroppedMessages++;
      }
      _lastLog++;
      _logs[_lastLog%NB_LOGS]._message = std::string(file_name_and_line) + std::string(message);
      _logs[_lastLog%NB_LOGS]._ts = ts;
      _logs[_lastLog%NB_LOGS]._nbFound = nbFound;
      _logs[_lastLog%NB_LOGS]._nbDropped = sendSyslog ? 0 : nbDropped + 1;
    }
    if (sendSyslog) {
      char droppedMessages[30];
      snprintf(droppedMessages, sizeof(droppedMessages), " [%d dropped msgs]", _nbDroppedMessages);
#ifdef ENABLE_SYS_LOG
      syslog(LOG_NOTICE, "%d|evse:%s%s", static_cast<uint32_t> (time(0)), fulllog, _nbDroppedMessages > 0 ? droppedMessages : "");
#endif
      _nbDroppedMessages = 0;
    }
  }
}


void log_(char * str) {
  if ( TraceDebug::isLogEnabled() ) {

#if ENABLE_LOG_SERVER && !_X86_
  static HostConnection hostConnection;

  if (hostConnection.IsConnected()) {
    // send to host
    hostConnection.WriteStr(str);
  } else
#endif
  {
  // write to standard output
  write(1, str, strlen(str));
  }
    }
}


inline timeval getTime() {
  timeval actualTime;

#if 0
  gettimeofday(&actualTime, 0);
#else
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
  actualTime.tv_sec = ts.tv_sec;
  actualTime.tv_usec = ts.tv_nsec / 1000;
#endif

  return actualTime;
}

timeval getTimestamp() {
  timeval actualTime;
  timeval res;
  static timeval startTime = getTime();

  actualTime = getTime();

  // return (actualTime - startTime);
  timersub(&actualTime, &startTime, &res);
  return res;
}


void logOCPP(char *format, ...) {
#ifdef ENABLE_SYS_LOG
  va_list ap;
  char str[USER_MSG_SIZE];

  va_start(ap, format);

  // format string
  vsnprintf(str, sizeof(str), format, ap);

  // write it to log output
  syslog(LOG_NOTICE, "%d|ocpp:%s", static_cast<uint32_t> (time(0)), str);

  va_end(ap);
#endif
}
