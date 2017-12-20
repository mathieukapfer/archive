/**
 * @File:   TimeManager.h
 * @Author: RLE12309
 *
 * @Date 8 janvier 2014
 */

#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H
#include <stdint.h>
#include <list>
#include <string>
using std::string;

#define HOUR_DURATION_IN_SEC 3600
#define DAY_DURATION_IN_SEC  (HOUR_DURATION_IN_SEC * 24)
#define WEEK_DURATION_IN_SEC  (DAY_DURATION_IN_SEC * 7)
#define MONTH_DURATION_IN_SEC  (DAY_DURATION_IN_SEC * 31)
#define YEAR_DURATION_IN_SEC  (DAY_DURATION_IN_SEC * 365)

#define USER_LOCALTIME_PATH "/mnt/datas/etc"
#define USER_LOCALTIME_FILE USER_LOCALTIME_PATH "/localtime"

class TimeManager {

 public:
  static void setTimezone(string filename);
  static bool SetDateTime(uint32_t timestamp);
  static uint32_t GetDateTime();

};

#endif /* TIMEMANAGER_H */
