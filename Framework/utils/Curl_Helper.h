/* 
 * File:   Curl_Helper.h
 * Author: open
 *
 * Created on 26 mai 2015, 15:19
 */

#ifndef CURL_HELPER_H
#define CURL_HELPER_H

#include <iostream>
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
// CURL
#include <curl/curl.h>

using std::string;

class Curl_Helper {
 public:
  Curl_Helper() {
  }

  virtual ~Curl_Helper() {
  }

  static int Curl_Upload_File(string url, string fileName, string pathFileName);
  static int sendCurlRequest_File(string url, string fileName);
  static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
 private:
  struct BufferStruct {
    char * buffer;
    size_t size;
  };
};

#endif /* CURL_HELPER_H */

