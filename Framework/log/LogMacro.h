#ifndef LOGMACRO_H
#define LOGMACRO_H

#define TRACEDEBUG_REGISTER(tag)                                        \
  static const int  *tracedebug_level = TraceDebug::inst()->getLogLevelEntry(tag); \
  static const char *tracedebug_tag = tag;

// Special macro for SMC code
// ===========================
#define TRACE(...)  {                                                 \
    TRACEDEBUG_REGISTER("[SMC]");                                     \
    LOG_INFO(__VA_ARGS__) ;                                           \
  }

#define TRACE_NOTICE(...)  {                                            \
    TRACEDEBUG_REGISTER("[SMC]");                                       \
    LOG_NOTICE(__VA_ARGS__) ;                                           \
  }

// Special macro to debug tracedebug...
#ifdef WEBSERVER
  #define PRINTF(...)    {void();}
#else
  #define PRINTF(...)    {printf(__VA_ARGS__);}
#endif


// Helper to log on change
#define DO_ON_CHANGE(type, val, fct)        \
  {                                         \
    static type persistent;                 \
    static bool firstCall = true;           \
    if ( firstCall || persistent != val ) { \
      firstCall = false;                    \
      persistent = val;                     \
      fct;                                  \
    }                                       \
  }



// User macro for condition
// =========================
#define _TEST_LOG_DEBUG_   (tracedebug_level && AFF_LOG_LEVEL_DEBUG   <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_INFO_    (tracedebug_level && AFF_LOG_LEVEL_INFO    <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_NOTICE_  (tracedebug_level && AFF_LOG_LEVEL_NOTICE  <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_WARNING_ (tracedebug_level && AFF_LOG_LEVEL_WARNING <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_ERROR_   (tracedebug_level && AFF_LOG_LEVEL_ERROR   <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_FATAL_   (tracedebug_level && AFF_LOG_LEVEL_FATAL   <= MASK_LOG_LSB(*tracedebug_level))
#define _TEST_LOG_FORCE_   (true)

#define TEST_LOG_DEBUG   if (_TEST_LOG_DEBUG_)
#define TEST_LOG_INFO    if (_TEST_LOG_INFO_)
#define TEST_LOG_NOTICE  if (_TEST_LOG_NOTICE_)
#define TEST_LOG_WARNING if (_TEST_LOG_WARNING_)
#define TEST_LOG_ERROR   if (_TEST_LOG_ERROR_)
#define TEST_LOG_FATAL   if (_TEST_LOG_FATAL_)
#define TEST_LOG_FORCE   if (_TEST_LOG_FORCE_)

// User macro with prefixed log
// ===========================
#if 1
  // NOTE : By remove LOG_DEBUG, you may save 200 kB on binary size (and slightly improve performance)

  #define LOG_DEBUG(...)   { TEST_LOG_DEBUG   { logSyslog(AFF_LOG_LEVEL_DEBUG,   LOG_LEVEL_DEBUG,   __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  #define LOG_INFO(...)    { TEST_LOG_INFO    { logSyslog(AFF_LOG_LEVEL_INFO,    LOG_LEVEL_INFO,    __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  #define LOG_NOTICE(...)  { TEST_LOG_NOTICE  { logSyslog(AFF_LOG_LEVEL_NOTICE,  LOG_LEVEL_NOTICE,  __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  #define LOG_WARNING(...) { TEST_LOG_WARNING { logSyslog(AFF_LOG_LEVEL_WARNING, LOG_LEVEL_WARNING, __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  #define LOG_ERROR(...)   { TEST_LOG_ERROR   { logSyslog(AFF_LOG_LEVEL_ERROR,   LOG_LEVEL_ERROR,   __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  #define LOG_FATAL(...)   { TEST_LOG_FATAL   { logSyslog(AFF_LOG_LEVEL_FATAL,   LOG_LEVEL_FATAL,   __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}
  // hack used to temporary force one specifique LOG (should not be used definitely)
  #define LOG_FORCE(...)   { TEST_LOG_FORCE   { logSyslog(AFF_LOG_LEVEL_FORCE,   LOG_LEVEL_FORCE,   __FILE__, __LINE__, tracedebug_tag, "", "\n", __VA_ARGS__); } else { void(); }}

#else
  #define LOG_DEBUG(...)   {}
  #define LOG_INFO(...)    {}
  #define LOG_NOTICE(...)  {}
  #define LOG_WARNING(...) {}
  #define LOG_ERROR(...)   {}
  #define LOG_FATAL(...)   {}
  // hask use for temporary force one specifique LOG (should not be used definitely)
  #define LOG_FORCE(...)   {}
#endif

// User macro without prefixed log
// ===============================
#define _LOG_DEBUG(LOG_STRING)   {TEST_LOG_DEBUG   log LOG_STRING;}
#define _LOG_INFO(LOG_STRING)    {TEST_LOG_INFO    log LOG_STRING;}
#define _LOG_NOTICE(LOG_STRING)  {TEST_LOG_NOTICE  log LOG_STRING;}
#define _LOG_WARNING(LOG_STRING) {TEST_LOG_WARNING log LOG_STRING;}
#define _LOG_ERROR(LOG_STRING)   {TEST_LOG_ERROR   log LOG_STRING;}
#define _LOG_FATAL(LOG_STRING)   {TEST_LOG_FATAL   log LOG_STRING;}

// User macro for enter and exit function
// =======================================
#define LOG_ENTER(...)   { TEST_LOG_DEBUG  { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s called (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_DEBUG,  LOG_LEVEL_DEBUG,  __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}
#define LOG_ENTER_(...)  { TEST_LOG_INFO   { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s called (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_INFO,   LOG_LEVEL_INFO,   __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}
#define LOG_ENTER__(...) { TEST_LOG_NOTICE { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s called (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_NOTICE, LOG_LEVEL_NOTICE, __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}

#define LOG_EXIT(...)   { TEST_LOG_DEBUG  { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s exits (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_DEBUG,  LOG_LEVEL_DEBUG,  __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}
#define LOG_EXIT_(...)  { TEST_LOG_INFO   { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s exits (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_INFO,   LOG_LEVEL_INFO,   __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}
#define LOG_EXIT__(...) { TEST_LOG_NOTICE { char prefix[50]; snprintf(prefix, sizeof(prefix), "%s exits (", __FUNCTION__); logSyslog(AFF_LOG_LEVEL_NOTICE, LOG_LEVEL_NOTICE, __FILE__, __LINE__, tracedebug_tag, prefix, ")\n", "" __VA_ARGS__); } else { void(); }}

#define LOG_OCPP(...) { logOCPP(__VA_ARGS__); LOG_NOTICE(__VA_ARGS__); }
#define LOG_OCPP_ERROR(...) { logOCPP(__VA_ARGS__); LOG_ERROR(__VA_ARGS__); }

#endif /* LOGMACRO_H */
