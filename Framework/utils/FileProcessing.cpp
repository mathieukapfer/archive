/* 
 * File:   FileProcessing.cpp
 * Author: open
 * 
 * Created on 26 mai 2015, 11:14
 */

#include "FileProcessing.h"
#ifndef WWWLITE
#include "ConfAccessor.h"
#include "ConnectivityConf.h"
#include "FwUpdateConf.h"
#include "OcppConfigFile.h"
#endif
#include "iniparser.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
extern "C" {
#include <sys/wait.h>
}
#include "TraceDebug.h"

TRACEDEBUG_REGISTER("[UTILS][FILE]")
#ifndef WWWLITE
string upper_string(const string& str) {
  string upper;
  transform(str.begin(), str.end(), back_inserter(upper), ::toupper);
  return upper;
}

/**
 * @brief create config fileName
 */
void FileProcessing::create(bool reportFile, bool updateFile, string fileName) {
  ofstream file(fileName.c_str());

  // write Evse Conf
  file << endl;
  file << "# Evse Conf" << endl;
  file << endl;
  ConfAccessor::getConfInst()->FlushToFile(&file);

  // write Ocpp Conf
  file << endl;
  file << "# Ocpp Conf" << endl;
  file << endl;
  OcppConfigFile::instance()->FlushToFile(&file);

  if (reportFile) {
    // write Report Conf
    file << endl;
    file << "# Report Conf" << endl;
    file << endl;
    ConnectivityConf::inst()->FlushToFile(&file);
  }

  if (updateFile) {
    // write Update Conf
    file << endl;
    file << "# Update Conf" << endl;
    file << endl;
    FwUpdateConf::inst()->FlushToFile(&file);
    file << endl;
  }

  file.close();
}

/**
 * @brief merge configFiles and slaveFile in configFiles
 */
void FileProcessing::merge(string configFiles, string slaveFile) {
  dictionary* ini;
  dictionary* ini_Slave;
  vector<SecKeyValue> differencesList;
  ini = iniparser_load(configFiles.c_str());
  ini_Slave = iniparser_load(slaveFile.c_str());
  if ((ini == NULL) || (ini_Slave == NULL)) {
    LOG_INFO("Config files are not present");
  } else {
    int nbSec = iniparser_getnsec(ini);
    for (int i = 0; i < nbSec; i++) {
      char* section = iniparser_getsecname(ini, i);
      char** secKeys = iniparser_getseckeys(ini, section);
      int nbKey = iniparser_getsecnkeys(ini, section);
      for (int j = 0; j < nbKey; j++) {
        char* value = iniparser_getstring(ini, secKeys[j], NULL);
        char* valueSlave = iniparser_getstring(ini_Slave, secKeys[j], NULL);

        if ((valueSlave != NULL) && (strcmp(value, valueSlave) != 0)) {
          char* key = strchr(secKeys[j], ':') + 1;
          SecKeyValue secKeyValue(section, key, valueSlave);
          differencesList.push_back(secKeyValue);
        }
      }

      free(secKeys);
    }
  }

  // cleanup dictionnary
  iniparser_freedict(ini);
  iniparser_freedict(ini_Slave);

  int size = differencesList.size();
  if (size) {
    ifstream ifs(configFiles.c_str());
    if (ifs.is_open()) {
      string contents;
      string line;
      int index = 0;
      string section = "[" + differencesList.at(index).section + "]";
      string key = differencesList.at(index).key;
      string value = differencesList.at(index).value;
      bool checkSection = true;
      while (getline(ifs, line)) {
        contents += line + "\n";
        if (index < size) {
          if (checkSection) {
            if (upper_string(line).find(upper_string(section)) != string::npos) {
              checkSection = false;
            }
          } else {
            if (upper_string(line).find(upper_string(key)) != string::npos) {
              size_t found = line.find("=");
              if (found != string::npos) {
                string _key = line.substr(0, found);
                // Remove spaces that are at end of the key
                found = _key.find_last_of(" \t");
                while ((found != string::npos) && (found == (_key.size() - 1))) {
                  _key = _key.substr(0, found);
                  found = _key.find_last_of(" \t");
                }

                contents += _key + "_2 = " + value + "\n";
              }

              index++;
              if (index < size) {
                string nextSection = "[" + differencesList.at(index).section + "]";
                key = differencesList.at(index).key;
                value = differencesList.at(index).value;
                if (section.compare(nextSection) != 0) {
                  section = nextSection;
                  checkSection = true;
                }
              }
            }
          }
        }
      }

      ifs.close();

      ofstream ofs(configFiles.c_str());
      ofs << contents;
      ofs.close();
    }
  }
}
#endif

bool FileProcessing::copyFile(string src, string dest) {
  ///@crw do not use fork+ sys cmd to copy file. Replace with standard C function

  bool ret = false;
  int pid = fork();
  if (pid == 0) {
    char *argv[] = { "/bin/cp", (char*) src.c_str(), (char*) dest.c_str(),
    NULL };
    LOG_DEBUG("CHILD : Execv CP evse (PID = %d PPID = %d) ", getpid(), getppid());
    execvp("/bin/cp", argv);
    LOG_INFO("Error: command cp cannot be launched");
    _exit(1);  // exit with error
  } else if (pid < 0) {  // fork failed
    LOG_INFO("fork() failed<br/>");
    _exit(1);
  } else {
    // father: wait for the child and report it's success/failure
    int status = 0;
    LOG_DEBUG("Father (pid = %d ) waiting for child (pid = %d) ", getpid(), pid);
    waitpid(pid, &status, 0);
    LOG_DEBUG("Father : end wait status = %d", status);
    ret = true;
  }
  return ret;
}
