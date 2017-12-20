/*
 * Class Application table level
 *
 * The inner table is filled by all 'TRACEDEBUG_REGISTER' called
 * and contains all tag know by the application code
 */
#ifndef APPLICATIONLEVELTAB_H
#define APPLICATIONLEVELTAB_H

#include "tracedebug.h"

class ApplicationLevelTab {

 public:
  ApplicationLevelTab();
  virtual ~ApplicationLevelTab() {};

  /*
   * Create and intialize a new entry in appLevelTab
   * return the reference to log level
   */
  int *getLogLevelEntry(const char * source, int logLevel);

  /*
   * Update the log level of an entry in appLevelTab
   */
  bool setLogLevelEntry(const char * tag, int logLevel);

  // deep accessor (return NULL if index out of bound)
  char * getTag(int index);

 private:

  // search a given tag in base and return its index
  int findTag(const char * tag);

  // application level item
  typedef struct {
    int logLevel;
    char tag[LOG_TABLE_TAG_LENGTH];
  } appLevelItemT;

  // application level table
  appLevelItemT _appLevelTab[LOG_TABLE_NB_ENTRIES];

  // application level table last index
  int _appLevelTabLastIndex;

};


#endif /* APPLICATIONLEVELTAB_H */
