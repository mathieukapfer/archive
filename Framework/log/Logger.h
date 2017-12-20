#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#include "OcppLib/LoggerI.h"

namespace Evse {
namespace Utils {

class Logger: public LoggerI {
 public:
  Logger();
  explicit Logger(char* p_tag);
  ~Logger();

  void log(const std::string& p_data);
  void log_debug(const std::string& p_file, const int& p_line, const std::string& p_data);
  void log_info(const std::string& p_file, const int& p_line, const std::string& p_data);
  void log_notice(const std::string& p_file, const int& p_line, const std::string& p_data);
  void log_error(const std::string& p_file, const int& p_line, const std::string& p_data);
  void log_fatal(const std::string& p_file, const int& p_line, const std::string& p_data);

 private:
  char* _tag;
};

} /* namespace Utils */
} /* namespace Evse */

#endif /* UTILS_LOGGER_H_ */
