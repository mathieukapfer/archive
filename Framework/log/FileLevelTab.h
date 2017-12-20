/*
 * Class FileLevelTab
 *
 * This class contains the file "log.cfg" 's memory representation
 * The inner table is filled by 'parseFile()' at start up
 */

#ifndef FILELEVELTAB_H
#define FILELEVELTAB_H

#include "tracedebug.h"

class FileLevelTab {
 public:
  FileLevelTab();
  virtual ~FileLevelTab() {};

  // init
  void parseFile();

  // accessor
  int getLogLevel_(const char * tag);
  int getGlobalLogLevel();

  // deep accessor (return NULL if index out of bound)
  char * getTag(int index);

  bool setLogLevel(const char* tag, const char * levelString);

 private:

  void parsePass1();
  void parsePass2();
  int parseLogLevel(const char *tampon);

  // cnf file related method
  FILE* fd;

  // File "log.cfg" memory representation
  // -----------------------------------------
  // - global log level
  int _globalLogLevel;

  // - log level by tag
  typedef struct {
    int indent;
    char tag[LOG_TABLE_TAG_LENGTH];
    int logLevel;
  } fileLevelItemT;

  fileLevelItemT _fileLevelTab[LOG_TABLE_NB_ENTRIES];

};


#endif /* FILELEVELTAB_H */
