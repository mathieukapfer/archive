
// Cfg File size limitation
// =========================
#define LOG_TABLE_NB_ENTRIES 300
#define LOG_TABLE_TAG_LENGTH 50

// Log level value
// ================
#define AFF_LOG_LEVEL_DEBUG    (6)
#define AFF_LOG_LEVEL_INFO     (5)
#define AFF_LOG_LEVEL_NOTICE   (4)
#define AFF_LOG_LEVEL_WARNING  (3)
#define AFF_LOG_LEVEL_ERROR    (2)
#define AFF_LOG_LEVEL_FATAL    (1)
#define AFF_LOG_LEVEL_FORCE    (0)
#define MASK_LOG_LSB(val) (val)

// Tag for each log level
// ======================
#define LOG_LEVEL_DEBUG    "DEBUG"
#define LOG_LEVEL_INFO     "INFO "
#define LOG_LEVEL_NOTICE   "NOTIC"
#define LOG_LEVEL_WARNING  "WARN "
#define LOG_LEVEL_ERROR    "ERROR"
#define LOG_LEVEL_FATAL    "FATAL"
#define LOG_LEVEL_FORCE    "FORCE"

// Conversion log level to string
// ===================================
char * getLogLevelString(int loglevel);

// Inner log
// ---------
//#define ENABLE_INNER_LOG

#ifdef ENABLE_INNER_LOG
#define INNER_LOG(...) printf("DBG:" __VA_ARGS__)
#else
#define INNER_LOG(...)
#endif
