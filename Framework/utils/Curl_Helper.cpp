/* 
 * File:   Curl_Helper.cpp
 * Author: open
 * 
 * Created on 26 mai 2015, 15:19
 */

#include "Curl_Helper.h"
#include "TraceDebug.h"
#include "stdlib.h"
#include "token.h"

TRACEDEBUG_REGISTER("[CONN][CURL]")

/**
 * 
 * @param url           : "protocol://user:pass@server:port"
 * @param fileName      : name of file
 * @param pathFileName  : path of file without filename
 * @return 
 */
int Curl_Helper::Curl_Upload_File(string url, string fileName, string pathFileName) {
  LOG_ENTER_();

  CURL *curl;
  CURLcode res;
  struct stat file_info;
  FILE *fd;

  string response = "";
  // init curl structs
  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;
  struct curl_slist *headerlist = NULL;
  // buffer for custom header list
  static const char buf[] = "Expect:";

  // urlUpload = url + fileName
  string urlUpload = "";

  // path = pathFileName + fileName
  string path = pathFileName + fileName;

  //ftp://user:password@host:port/path in which the parts user:password@, :password or :port may be excluded

  fd = fopen(path.c_str(), "rb"); /* open file to upload */
  if (!fd) {

    return 1; /* can't continue */
  }

  /* to get the file size */
  if (fstat(fileno(fd), &file_info) != 0) {

    return 1; /* can't continue */
  }

  // Remove curl global init in main AS NOT THREAD SAFE !!!
  // curl_global_init(CURL_GLOBAL_ALL);

  if (url.substr(0, 4) == "http") {
    // Fill in the file upload field
    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "fichier", CURLFORM_FILE,
                 const_cast<char*>(path.c_str()), CURLFORM_END);

    curl = curl_easy_init();

    // initalize custom header list (stating that Expect: 100-continue is not wanted
    headerlist = curl_slist_append(headerlist, buf);
  } else {
    curl = curl_easy_init();
  }

  if (curl) {

    //if http protocol, we use the POST form
    if (url.substr(0, 4) == "http") {
      urlUpload = url;
      /**@satisfies{EV_Parking_Board_CODE_65,EV_Parking_Board_CMPR_008_32,T} : treat HTTP POST request*/
      LOG_INFO("uploadFile as http url = %s", urlUpload.c_str());
      LOG_INFO("uploadFile path = %s", path.c_str());

      // set the url
      curl_easy_setopt(curl, CURLOPT_URL, const_cast<char*>(url.c_str()));
      // set header list
      curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
      // set the post form
      curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
      /* upload to this place */
      curl_easy_setopt(curl, CURLOPT_URL, urlUpload.c_str());
      // set up the cookie
      std::string cookie = "CURLTOKEN=" + std::string(CURLTOKEN);
      curl_easy_setopt(curl, CURLOPT_COOKIE, const_cast<char*>(cookie.c_str()));
    } else {
      urlUpload = url + fileName;

      LOG_INFO("uploadFile not http url = %s", urlUpload.c_str());
      LOG_INFO("uploadFile path = %s", path.c_str());

      /* tell it to "upload" to the URL */
      curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
      /**@satisfies{EV_Parking_Board_CODE_66,EV_Parking_Board_CMPR_008_31,T} : treat FTP PUT request*/
      curl_easy_setopt(curl, CURLOPT_PUT, 1L);

      /* upload to this place */
      curl_easy_setopt(curl, CURLOPT_URL, urlUpload.c_str());

      /* set where to read from (on Windows you need to use READFUNCTION too) */
      curl_easy_setopt(curl, CURLOPT_READDATA, fd);

      /* and give the size of the upload (optional) */
      curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t ) file_info.st_size);

    }
    /* enable verbose for easier tracing */
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 20);

    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res == CURLE_OK) {
      LOG_INFO("Upload file %s is Done", fileName.c_str());
    } else {
      LOG_ERROR("Cannot upload file %s ", fileName.c_str());
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    if (url.substr(0, 4) == "http") {
      curl_formfree(formpost);
      // free slist
      curl_slist_free_all(headerlist);
    }

  }

  fclose(fd); /* close the local file */

  // Remove curl global init in main AS NOT THREAD SAFE !!!
  // curl_global_cleanup();

  LOG_EXIT_("%d", res);
  return res;
}

/**
 * Send request to url and receive response in fileName
 * @param url
 * @param fileName
 * @return 
 */
int Curl_Helper::sendCurlRequest_File(string url, string fileName) {
  LOG_ENTER("url:%s fileName:%s", url.c_str(), fileName.c_str());
  int ret = 0;
  CURL * myHandle;
  CURLcode result;
  // We’ll store the result of CURL’s webpage retrieval, for simple error checking.
  struct BufferStruct output;
  // Create an instance of out BufferStruct to accept LCs output
  output.buffer = NULL;
  output.size = 0;
  myHandle = curl_easy_init();

  // Passing the function pointer to LC
  curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  // Passing our BufferStruct to LC
  curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, (void * ) &output);
  // set up the url
  curl_easy_setopt(myHandle, CURLOPT_URL, const_cast<char*>(url.c_str()));
  // set up the cookie
  std::string cookie = "CURLTOKEN=" + std::string(CURLTOKEN);
  curl_easy_setopt(myHandle, CURLOPT_COOKIE, const_cast<char*>(cookie.c_str()));
  // fix connection timeout to 10sec
  curl_easy_setopt(myHandle, CURLOPT_CONNECTTIMEOUT, 10);
  result = curl_easy_perform(myHandle);
  curl_easy_cleanup(myHandle);
  // open the file
  FILE * fp;
  fp = fopen(const_cast<char*>(fileName.c_str()), "w");
  if (!fp) {
    LOG_ERROR("Cannot open file %s for writing ", const_cast<char*> (fileName.c_str()));
    ret = 1;
  } else {
    if (result == CURLE_OK) {
      // finalize file 
      LOG_INFO("CurlRequest OK");
      fprintf(fp, output.buffer);
      fclose(fp);
      // reset buffer 
      if (output.buffer) {
        free(output.buffer);
        output.buffer = 0;
        output.size = 0;
      }
    } else {
      //treat error

      LOG_ERROR("CurlRequest KO (%s)", url.c_str());
    }

    ret = result;
  }

  LOG_EXIT("%d", ret);
  return ret;
}

/**
 * @brief get curl response and set it into a fiel in memory
 * @param ptr
 * @param size
 * @param nmemb
 * @param data
 * @return 
 */
size_t Curl_Helper::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  size_t realsize = size * nmemb;

  struct BufferStruct * mem = (struct BufferStruct *) data;

  mem->buffer = (char*) realloc(mem->buffer, mem->size + realsize + 1);

  if (mem->buffer) {
    memcpy(&(mem->buffer[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0;
  }
  return realsize;
}

