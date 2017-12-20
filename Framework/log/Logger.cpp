#include "Logger.h"
#include "TraceDebug.h"
#include "TraceDebug.h"
#include <stdarg.h> // va_list
#include <stdio.h> // printf
#include <unistd.h> // write

#ifdef ENABLE_LOG_SERVER
#include "HostConnection.h"
#endif

namespace Evse {
namespace Utils {

Logger::Logger() :
  _tag("[OCPP][OCPPLIB]]") {
}

Logger::Logger(char* p_tag) :
  _tag(p_tag) {
}

Logger::~Logger() {
}

void Logger::log_debug(const std::string& p_file, const int& p_line, const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_DEBUG, LOG_LEVEL_DEBUG, (char *) p_file.c_str(), p_line, _tag, "", "\n", tmp);
}
void Logger::log_info(const std::string& p_file, const int& p_line, const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_INFO, LOG_LEVEL_INFO, (char *) p_file.c_str(), p_line, _tag, "", "\n", tmp);
}
void Logger::log_notice(const std::string& p_file, const int& p_line, const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_NOTICE, LOG_LEVEL_NOTICE, (char *) p_file.c_str(), p_line, _tag, "", "\n", tmp);
}
void Logger::log_error(const std::string& p_file, const int& p_line, const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_ERROR, LOG_LEVEL_ERROR, (char *) p_file.c_str(), p_line, _tag, "", "\n", tmp);
}
void Logger::log_fatal(const std::string& p_file, const int& p_line, const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_FATAL, LOG_LEVEL_FATAL, (char *) p_file.c_str(), p_line, _tag, "", "\n", tmp);
}

void Logger::log(const std::string& p_data) {
  char tmp[255];
  snprintf(tmp, sizeof(tmp), "%s", p_data.c_str());
  logSyslog(AFF_LOG_LEVEL_NOTICE, LOG_LEVEL_NOTICE, "undef", 0, _tag, "", "\n", tmp);
}

} /* namespace Utils */
} /* namespace Evse */
