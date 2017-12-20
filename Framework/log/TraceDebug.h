#ifndef TRACEDEBUG_INCLUDED
#define TRACEDEBUG_INCLUDED


/* ******************************
 * Log your application !
 * ******************************

 * Code extract:
 * ---------------

  #include "TraceDebug.h"

  TRACEDEBUG_REGISTER("[TOTO]");

  int main(int argc, char argv)
  {
    LOG_DEBUG("hello debug");
    LOG_INFO ("hello info");
  }

 * Log configuration file :
 * ---------------------------

 // LOG_LEVEL_DEBUG, LOG_LEVEL_INFO, LOG_LEVEL_NOTICE, LOG_LEVEL_WARNING, LOG_LEVEL_ERROR, LOG_LEVEL_FATAL

[GLOBAL] LOG_LEVEL_NOTICE
[TOTO]   LOG_LEVEL_INFO

 * Console output :
 * -----------------

 test.cpp:8:          [INFO   ][TOTO]      hello info

*/
#ifndef _X86_
#define ENABLE_SYS_LOG
#endif

#include "tracedebug.h"
#include "LogMacro.h"
#include "LogFormater.h"
#include "ApplicationLevelTab.h"
#include "FileLevelTab.h"

// Internal methods
void refreshLogLevelProxy(int signum);

class TraceDebug {

 public:

  static TraceDebug *inst() {
    if (!_globalObject) _globalObject = new (TraceDebug);

    return _globalObject;
  }

  ~TraceDebug();

  int *getLogLevelEntry(const char * tag);

  void refreshLogLevel(int signum);

  static void disableLog() {_logEnable = false;}
  static void enableLog() {_logEnable = true;}
  static bool isLogEnabled() {return _logEnable;}
  static std::string getMasterSlavePrefix() {return _isMasterDefined ? (_isMaster ? "|" : "~") : "?";}
  static void setIsMaster(bool p_isMaster) {_isMaster = p_isMaster; _isMasterDefined = true;}

  bool setLogLevel(const char * tag, const char * levelString);

 private:

  TraceDebug();

  int getLogLevel(const char * source);

  bool initialise();

  void refreshLogLevelEntry();

  static TraceDebug *_globalObject;

  static bool _logEnable;
  static bool _isMaster;
  static bool _isMasterDefined;

  /* File "log.cfg" memory representation
   * This table is filled by 'parseFile()' at start up
   */
  FileLevelTab _fileLevelTab;

  /* Application table level
   * This table is filled by all 'TRACEDEBUG_REGISTER' called
   */
  ApplicationLevelTab _applicationLevelTab;

};

#endif
