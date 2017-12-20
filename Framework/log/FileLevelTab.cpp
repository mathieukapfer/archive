#include <stdio.h>
#include <string.h>

#include "FileLevelTab.h"
#include "LogMacro.h" // for PRINTF

#define LOG_CNF_FILE    "log.cfg"

FileLevelTab::FileLevelTab():
  fd(NULL),
  _globalLogLevel(AFF_LOG_LEVEL_NOTICE) {
  memset(_fileLevelTab, 0, LOG_TABLE_NB_ENTRIES * sizeof(fileLevelItemT));
}


// Parse file related fct
// (get from previous project Linky-ERDF)

void FileLevelTab::parseFile() {

  int res;

  // reset all data
  memset(_fileLevelTab, 0, LOG_TABLE_NB_ENTRIES * sizeof(fileLevelItemT));

  // open file
  fd = fopen(LOG_CNF_FILE, "r");
  if (fd == NULL) {
    PRINTF("<!--TRACEDEBUG ERROR   : Erreur ouverture fichier de conf %s-->\n", LOG_CNF_FILE);
    return;
  }

  // pass 1 : read each line
  parsePass1();
  // pass 2 : compute level (inherate upper log level from parent)
  parsePass2();

  // close file
  res = fclose(fd);
  if (res < 0) {
    PRINTF("<!--TRACEDEBUG ERROR : Erreur fermeture fichier de conf %d-->\n", res);
  }

}

// FileLevelTab::parseLines()
// ==========================
// File : log.cfg =
// -------
// [GLOBAL]  LOG_LEVEL_ERROR
// [MODULE1]
//     [SUBMODULE1] LOG_LEVEL_INFO
//     [SUBMODULE2] LOG_LEVEL_INFO
//
// provide table: _fileLevelTab =
// --------------
// {{indent = 0, tag = "[GLOBAL", '\000' <repeats 92 times>, logLevel = 2}
//  {indent = 0, tag = "[MODULE1", '\000' <repeats 91 times>,    logLevel = -1},
//  {indent = 1, tag = "[SUBMODULE1", '\000' <repeats 88 times>, logLevel = 8},
//  {indent = 0, tag = "[MODULE2", '\000' <repeats 91 times>, logLevel = -1},
//  {indent = 1, tag = "[SUBMODULE2", '\000' <repeats 88 times>, logLevel = 8},
//  { indent = 0, tag = '\000' <repeats 99 times>, logLevel = 0} <repeats 90 times>}

void FileLevelTab::parsePass1() {
  char buf[200];
  char *str = buf;
  int tabIndex = 0;
  int inputFileLine = 0;
  char* ptrsave = NULL;
  while (fgets(str, sizeof(buf), fd) != NULL)
    if (tabIndex < LOG_TABLE_NB_ENTRIES) {
      // get nb white char (tag or space)
      int pos = 0;
      while ( ( str[pos] == '\t' || str[pos] == ' ') && (str[pos] != '\0') ) {
        pos++;
      }
      str += pos;  // go forward

      // file pointer (for debug purpose)
      inputFileLine++;

      // get tags
      if (str[0] == '[') {
        char * token;
        token = strtok_r(str, "]", &ptrsave);

        if (token && token[0] == '[') {
          _fileLevelTab[tabIndex].indent = (pos > 0); // force to only one sub level supported
          strncpy(_fileLevelTab[tabIndex].tag, token, LOG_TABLE_TAG_LENGTH);

          token = strtok_r(NULL, "]", &ptrsave);  // go forward
        }

        // get level
        _fileLevelTab[tabIndex].logLevel = parseLogLevel(token);

        tabIndex++;

      }

    } else {
      PRINTF("<!--Too many entries in file %s, ligne %d ignored:'%s')--> \n", LOG_CNF_FILE,
      __LINE__,
             str);
    }

}
/* Parse log level string
 * return log level int or -1  if not valid
 */
int FileLevelTab::parseLogLevel(const char *str) {
  int log_lvl = -1;

  if (str) {

    // first syntaxe
    if (strstr(str, "LOG_LEVEL_DEBUG")) log_lvl = AFF_LOG_LEVEL_DEBUG;
    else if (strstr(str, "LOG_LEVEL_INFO")) log_lvl = AFF_LOG_LEVEL_INFO;
    else if (strstr(str, "LOG_LEVEL_NOTICE")) log_lvl = AFF_LOG_LEVEL_NOTICE;
    else if (strstr(str, "LOG_LEVEL_WARNING")) log_lvl = AFF_LOG_LEVEL_WARNING;
    else if (strstr(str, "LOG_LEVEL_ERROR")) log_lvl = AFF_LOG_LEVEL_ERROR;
    else if (strstr(str, "LOG_LEVEL_FATAL")) log_lvl = AFF_LOG_LEVEL_FATAL;
    // second syntaxe
    else if (strstr(str, " DEBUG")) log_lvl = AFF_LOG_LEVEL_DEBUG;
    else if (strstr(str, " INFO")) log_lvl = AFF_LOG_LEVEL_INFO;
    else if (strstr(str, " NOTICE")) log_lvl = AFF_LOG_LEVEL_NOTICE;
    else if (strstr(str, " WARNING")) log_lvl = AFF_LOG_LEVEL_WARNING;
    else if (strstr(str, " ERROR")) log_lvl = AFF_LOG_LEVEL_ERROR;
    else if (strstr(str, " FATAL")) log_lvl = AFF_LOG_LEVEL_FATAL;
  }

  return log_lvl;
}

// pass 2 :
// - compute log level (inherate upper log level from parent)
// - concat tags
#define MAX_INDENT_VALUE 10

void FileLevelTab::parsePass2() {
  int tableIndex = 0;
  int logLevelTable[MAX_INDENT_VALUE];
  char* tags[MAX_INDENT_VALUE];

  memset(logLevelTable, 0, sizeof(int) * MAX_INDENT_VALUE);
  memset(tags, 0, sizeof(char *) * MAX_INDENT_VALUE);

  // first : get global log level
  while ((tableIndex < LOG_TABLE_NB_ENTRIES) && _fileLevelTab[tableIndex].tag[0]) {
    if ( (_fileLevelTab[tableIndex].indent == 0) &&
         (strcmp(_fileLevelTab[tableIndex].tag, "[GLOBAL") == 0) &&
         (_fileLevelTab[tableIndex].logLevel != -1) ) {
      _globalLogLevel = _fileLevelTab[tableIndex].logLevel;
      PRINTF("<!--[GLOBAL]:%s -->\n", getLogLevelString(_globalLogLevel));
      break;
    } else {
      tableIndex++;
    }
  }

  tableIndex = 0;

  // second : inherite log level
  while ((tableIndex < LOG_TABLE_NB_ENTRIES) && _fileLevelTab[tableIndex].tag[0]) {
    // tag name in not null
    int indent = _fileLevelTab[tableIndex].indent;

    // build logLevel table
    if (indent < MAX_INDENT_VALUE) {
      // save log level
      logLevelTable[indent] = _fileLevelTab[tableIndex].logLevel;

      // close the missing braket of tag
      strncat(_fileLevelTab[tableIndex].tag, "]", LOG_TABLE_TAG_LENGTH);
      // save tag addr
      tags[indent] = _fileLevelTab[tableIndex].tag;
    }

    // inherite the upper logLevel
    while (indent > 0) {

      // inherite log level from parent if not defined
      if (_fileLevelTab[tableIndex].logLevel == 0) {
        _fileLevelTab[tableIndex].logLevel = logLevelTable[indent - 1];
      }

      // concat parent tag in reverse order
      {
        // - save it
        char tmp[LOG_TABLE_TAG_LENGTH];
        tmp[0] = '\0';
        strncpy(tmp, _fileLevelTab[tableIndex].tag, LOG_TABLE_TAG_LENGTH);
        // - erase it
        _fileLevelTab[tableIndex].tag[0] = '\0';
        // - concat it
        strncat(_fileLevelTab[tableIndex].tag, tags[indent - 1], LOG_TABLE_TAG_LENGTH);
        strncat(_fileLevelTab[tableIndex].tag, tmp, LOG_TABLE_TAG_LENGTH);
      }

      // next
      indent--;
    }

    // inherite global loglevel if not yet defined
    if (_fileLevelTab[tableIndex].logLevel == 0) {
      _fileLevelTab[tableIndex].logLevel = _globalLogLevel;
    }

    tableIndex++;
  }
}

/*
 * Read log level from file representation
 */
int FileLevelTab::getLogLevel_(const char * tag) {
  int ret = -1;
  int tableIndex = 0;
  while (_fileLevelTab[tableIndex].tag[0])
    if (strcmp(_fileLevelTab[tableIndex].tag, tag) == 0) {
      ret = (_fileLevelTab[tableIndex].logLevel);
      break;
    } else {
      tableIndex++;
    }
  return ret;
}

int FileLevelTab::getGlobalLogLevel() {
  return _globalLogLevel;
}

char * FileLevelTab::getTag(int tableIndex) {
  char * ret = NULL;

  if ( (tableIndex < LOG_TABLE_NB_ENTRIES) &&
       (_fileLevelTab[tableIndex].tag[0] != 0) ) {
      ret = _fileLevelTab[tableIndex].tag;
    }

  return ret;
}

bool FileLevelTab::setLogLevel(const char* tag, const char * levelString) {
  int ret = false;
  int tabIndex = 0;

  // prefix levelString with space to be able to call parseLogLevel
  // parseLogLevel accepts "LOG_LEVEL_DEBUG" or " DEBUG", but not "LOG_LEVEL_xDEBUG"
  char levelStringWithSpace[10];
  snprintf(levelStringWithSpace, sizeof(levelStringWithSpace), " %s", levelString);
  int logLevel = parseLogLevel(levelStringWithSpace);

  while (_fileLevelTab[tabIndex].tag[0]) {
    if (strcmp(_fileLevelTab[tabIndex].tag, tag) == 0) {
      _fileLevelTab[tabIndex].logLevel = logLevel;
      ret = true;
      break;
    } else {
      tabIndex++;
    }
  }
  if (!ret && tabIndex < LOG_TABLE_NB_ENTRIES - 1) {
    // tag not found in current _fileLevelTab, add it at the end
    if (tag[0] == '[') {
      _fileLevelTab[tabIndex].indent = 0;
      strncpy(_fileLevelTab[tabIndex].tag, tag, LOG_TABLE_TAG_LENGTH);
      _fileLevelTab[tabIndex].logLevel = logLevel;
      tabIndex++;
      ret = true;
    }
  }
  return ret;
}
