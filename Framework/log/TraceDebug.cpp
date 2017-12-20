#include <signal.h>
#include <stdarg.h> /* pour va_list */
#include <unistd.h> /* pour write */
#include <string>
#include <time.h>
#include "TraceDebug.h"
#include "config.h"
#include <syscall.h>


#ifdef  ENABLE_SYS_LOG
#include <syslog.h>
#endif

#ifdef ENABLE_LOG_SERVER
#include "HostConnection.h"
#endif

bool TraceDebug::_logEnable = true;
bool TraceDebug::_isMaster = false;
bool TraceDebug::_isMasterDefined = false;

TraceDebug *TraceDebug::_globalObject = 0;

TraceDebug::TraceDebug():
  _fileLevelTab(),
  _applicationLevelTab() {
  initialise();
}

TraceDebug::~TraceDebug() {
#ifdef ENABLE_SYS_LOG
  closelog();
#endif
  // log("TraceDebug Descructor called\n");
  _globalObject = NULL;

}


/*
 * Take the log level of tag, tag's parent or eventually the global one
 * (tag'parent of [MODULE][SUBMODUL] is [MODULE])
 */
int TraceDebug::getLogLevel(const char * source) {
  int ret = _fileLevelTab.getLogLevel_(source);
  if (ret != -1) {
    PRINTF("<!--%s:%s -->\n", source, getLogLevelString(ret));
  }

  // try with first label
  if ( ret == -1 ) {
    char buf[LOG_TABLE_TAG_LENGTH];
    strncpy(buf, source, LOG_TABLE_TAG_LENGTH);
    *strchrnul(buf+1, '[') = '\0';
    ret = _fileLevelTab.getLogLevel_(buf);
    if (ret != -1) {
      PRINTF("<!--%s:%s (from %s)-->\n", source, getLogLevelString(ret), buf);
    }
  }

  // last chance: get global level
  if ( ret == -1 ) {
    ret = _fileLevelTab.getGlobalLogLevel();
    // PRINTF("<!-- force tag %s to GLOBAL-->\n", source);
  }

  return ret;
}


bool TraceDebug::initialise() {

  // register signal handle
  signal(SIGUSR1, refreshLogLevelProxy);

  // parse cnf file
  _fileLevelTab.parseFile();

#ifdef ENABLE_SYS_LOG
//  openlog( "cgiServer", LOG_NDELAY|LOG_PID, LOG_DAEMON );
  openlog("EVSE_LOG", 0, LOG_DAEMON);
#endif
  return true;
}

void TraceDebug::refreshLogLevel(int signum) {
  PRINTF("<!--TraceDebug::Refresh called (by sig:%d)-->\n", signum);

  // read log config
  _fileLevelTab.parseFile();

  // update log level table
  refreshLogLevelEntry();

}

void refreshLogLevelProxy(int signum) {
  TraceDebug::inst()->refreshLogLevel(signum);
}


/*
 * Provide new entry in appLevelTab
 */
int *TraceDebug::getLogLevelEntry(const char * tag) {
  return _applicationLevelTab.getLogLevelEntry
    (// create this tag
      tag,
     // initialize it with the reference of the value found in log.cnf
     getLogLevel(tag));
}

/*
 * Update log level in appLevelTab
 */
void TraceDebug::refreshLogLevelEntry() {

  int tableIndex = 0;
  bool status = true;

  // init loop at the beginning
  char * currenttag = _applicationLevelTab.getTag(tableIndex);

  // foreach entry in config file
  while ( currenttag ) {
    // update the app level tab
    status &= _applicationLevelTab.setLogLevelEntry(
      currenttag, getLogLevel(currenttag));
    currenttag = _applicationLevelTab.getTag(++tableIndex);
  }

  if (status) {
    PRINTF("<!-- refreshLogLevelEntry OK -->\n");
  } else {
    PRINTF("<!-- refreshLogLevelEntry KO -->\n");
  }
}


bool TraceDebug::setLogLevel(const char * tag, const char * levelString) {
  bool ret = _fileLevelTab.setLogLevel(tag, levelString);
  if (ret) {
    refreshLogLevelEntry();
  }
  return ret;
}
