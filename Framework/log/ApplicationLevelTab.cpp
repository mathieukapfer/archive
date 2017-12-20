#include <stdio.h>
#include <string.h>
#include <string>

#include "ApplicationLevelTab.h"

/*
 * Cons
 */
ApplicationLevelTab::ApplicationLevelTab():
  _appLevelTabLastIndex(0) {
  memset(_appLevelTab, 0, LOG_TABLE_NB_ENTRIES * sizeof(appLevelItemT));
}

/*
 * Get the log level of the corresponding to 'tag',
 * create and intialize it in needed
 * return the reference to log level
 */
int *ApplicationLevelTab::getLogLevelEntry(const char * tag, int logLevel) {
  int currentIndex;

  // check if it is a new tag
  currentIndex = findTag(tag);

  // if not found
  if ( currentIndex < 0 ) {
    // take a new one
    currentIndex = _appLevelTabLastIndex++;
    // fill it
    if (currentIndex < LOG_TABLE_NB_ENTRIES) {
      _appLevelTab[currentIndex].logLevel = logLevel;
      strncpy(_appLevelTab[currentIndex].tag, tag, LOG_TABLE_TAG_LENGTH);
    } else {
      printf("<!-- index out of bound (%d>%d)-->\n", currentIndex, LOG_TABLE_NB_ENTRIES);
      _appLevelTabLastIndex--;
    }
  }

  INNER_LOG("getLogLevelEntry: index %d : %s\n", currentIndex, tag);

  // return reference of the pointer to log level
  return &(_appLevelTab[currentIndex].logLevel);
}

/*
 * Update the log level of an entry in appLevelTab
 */
bool ApplicationLevelTab::setLogLevelEntry(const char * tag, int logLevel) {
  int currentIndex = 0;
  bool status = false;

  // search the entry with same tag
  currentIndex = findTag(tag);
  if (currentIndex >= 0) {
    // update it
    _appLevelTab[currentIndex].logLevel = logLevel;
    status = true;
  }

  if (status) {
    INNER_LOG("setLogLevelEntry %s as %s OK (index:%d)\n", tag, getLogLevelString(logLevel), currentIndex);
  } else {
    INNER_LOG("setLogLevelEntry %s KO\n", tag);
  }

  return status;
}

/*
 * Search tag in appLevelTab
 * return index or -1 if not found
 */
int ApplicationLevelTab::findTag(const char * tag) {
  int currentIndex = 0;
  int ret = -1;

  while ( currentIndex < _appLevelTabLastIndex ) {
    if (strcmp(_appLevelTab[currentIndex].tag, tag) == 0) {
      ret = currentIndex;
      break;
    } else {
      currentIndex++;
    }
  }

  return ret;
}

// deep accessor (return NULL if index out of bound)
char * ApplicationLevelTab::getTag(int index) {
  char *ret = NULL;
  if (index < _appLevelTabLastIndex) {
    ret =  _appLevelTab[index].tag;
  }
  return ret;
}
