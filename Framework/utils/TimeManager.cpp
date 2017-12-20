/**
 * @File:   TimeManager.cpp
 * @Author: RLE12309
 *
 * @Date 8 janvier 2014
 */

#include "TimeManager.h"
#include <Time.h>
#include <ctime>
#include <errno.h>
#include <cstdlib>
#include <fstream>
#include <unistd.h>
#include <dirent.h>

// for mkdir :
#include <sys/stat.h>
#include <sys/types.h>

#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[MODEL][TIME]")
#define TIMEZONE_DIR_BSP "/usr/share/zoneinfo/"
#define TIMEZONE_DIR_MANUAL "/mnt/datas/usr/share/zoneinfo/"

/**
 *
 * @param timestamp
 * @return
 */
bool TimeManager::SetDateTime(uint32_t timestamp) {
  LOG_ENTER("%d", timestamp);
  bool ret = true;
  // fill with seconds
  timespec tm;
  tm.tv_nsec = 0;
  tm.tv_sec = timestamp;
  // set time
#ifndef TESTU
  if (clock_settime(CLOCK_REALTIME, &tm) == -1) {
    LOG_ERROR("Error while setting up sys/dateTime : %s", strerror(errno));
    ret = false;
  }
#endif
  LOG_EXIT("%s", ret ? "true" : "false");
  return ret;
}

/**
 *
 * @return
 */
uint32_t TimeManager::GetDateTime() {
  LOG_ENTER();
  uint32_t retTime = 0;
  timespec tmspec;
  // get time from system
  if (clock_gettime(CLOCK_REALTIME, &tmspec) != -1) {
    // in seconde
    retTime = tmspec.tv_sec;
  } else {
    LOG_ERROR("Error while getting up sys/dateTime : %s", strerror(errno));
  }
  // return timestamp
  LOG_EXIT("%d", retTime);
  return retTime;
}

bool ifDirExist(string path) {
  LOG_ENTER("%s", path.c_str());
  bool ret = true;
  if (opendir(path.c_str()) == NULL) {
    ret = false;
  }
  LOG_EXIT("%s", ret ? "True" : "False");
  return ret;
}

void TimeManager::setTimezone(string filename) {
  LOG_ENTER_("%s", filename.c_str());
  string path = "";

  // define zone info path
  if (ifDirExist(TIMEZONE_DIR_BSP)) {
    path = TIMEZONE_DIR_BSP;
  } else if (ifDirExist(TIMEZONE_DIR_MANUAL)) {
    path = TIMEZONE_DIR_MANUAL;
  }

  // check local dir
  if (!ifDirExist(USER_LOCALTIME_PATH)) {
    mkdir(USER_LOCALTIME_PATH, 0777);
  }

  // do the work
  if (!path.empty()) {
    string userLocalTime = path + filename;
    LOG_INFO("Timezone to set = %s", userLocalTime.c_str());
    //----------------------------------
    // Copy new file
    // set src/dst
    LOG_INFO("src %s => dest %s", userLocalTime.c_str(), USER_LOCALTIME_FILE);
    std::ifstream src(userLocalTime.c_str(), std::ios::binary);
    std::ofstream dst(USER_LOCALTIME_FILE, std::ios::binary);
    // copy
    dst << src.rdbuf();
    dst.close();
    src.close();
  }
  LOG_EXIT_("");
}
