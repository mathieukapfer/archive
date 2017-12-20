/* 
 * File:   FileProcessing.h
 * Author: open
 *
 * Created on 26 mai 2015, 11:14
 */

#ifndef FILEPROCESSING_H
#define FILEPROCESSING_H

#include <string>
#include <vector>

using namespace std;

#define CONFIG_FILES_NAME "configFiles.cfg"
#define CONFIG_FILES_PATH "/tmp/"
#define CONFIG_FILES_NAME_PATH CONFIG_FILES_PATH CONFIG_FILES_NAME
#define CONFIG_FILES_NAME_PATH_SLAVE CONFIG_FILES_PATH "configFiles.cfg_slave"

struct SecKeyValue {
  string section;
  string key;
  string value;
  SecKeyValue(string s, string k, string v) :
      section(s),
      key(k),
      value(v) {
  }
};

class FileProcessing {
 public:
#ifndef WWWLITE
  // create config fileName
  static void create(bool report = true, bool update = true, string fileName = CONFIG_FILES_NAME_PATH);
  // merge configFiles and slaveFile in configFiles
  static void merge(string configFiles = CONFIG_FILES_NAME_PATH, string slaveFile =
  CONFIG_FILES_NAME_PATH_SLAVE);
#endif
  static bool copyFile(string src, string dest);
 private:

};

#endif /* FILEPROCESSING_H */

