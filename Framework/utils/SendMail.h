/* 
 * File:   SendMail.h
 * Author: romain
 *
 * Template to send mail with curl
 * WARNING : This template use a static method and attribut
 * To separate static in all instanciation, use different dummytype
 * 
 * Created on 10 avril 2014, 15:50
 */

#ifndef SENDMAIL_H
#define SENDMAIL_H

#include <string>
#include <vector>
#include <curl/curl.h>
#include <stdio.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <stdio.h>


#include "TraceDebug.h"

using namespace std;

/********************************/
/* Useless typedef for template */
typedef char dummy1;
typedef double dummy2;
/* ADD YOUR DUMMY TYPE HERE     */

/********************************/

struct upload_status {
  int lines_read;
};

typedef struct {
#if 1
  //For Broadcast alarm
  vector<string> receivers;
#else
  string receiver;
#endif
  string subject;
} mailHeader;

typedef struct {
  string sender;
  string serverUrl;
  string user;
  string pwd;
  bool authent;
} mailServerInfo;

template<class Type_A>
class SendMail {
 public:
  SendMail();
  ~SendMail();
  int sendCUrlMail(const mailServerInfo& serverInfo, const mailHeader& header, const string& message, const string& filepath = "", const string& filename = "");
 private:
  static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp);
  static vector<string> payload_text;
  bool computeMessage(const string& sender, const mailHeader& header, const string& message, const string& filepath = "", const string& filename = "");
  string qEncode(string stringToEncode);
};

template<class Type_A>
vector<string> SendMail<Type_A>::payload_text;

template<class Type_A>
SendMail<Type_A>::SendMail() {
}

template<class Type_A>
SendMail<Type_A>::~SendMail() {

}

template<class Type_A>
size_t SendMail<Type_A>::payload_source(void *ptr, size_t size, size_t nmemb, void *userp) {
  TRACEDEBUG_REGISTER("[CURL][PAYLOAD]");
  LOG_ENTER();

  struct upload_status *upload_ctx = (struct upload_status *) userp;
  const char *data;

  if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
    return 0;
  }
  if ((unsigned int) (upload_ctx->lines_read) < payload_text.size()) {
    data = payload_text.at(upload_ctx->lines_read).c_str();

    if (data) {
      size_t len = strlen(data);
      memcpy(ptr, data, len);
      upload_ctx->lines_read++;

      TEST_LOG_DEBUG {
        for (unsigned int i = 0; i < len; i++) {
          log("%x", *((char *) ptr + i));
        }
        log("\n");
      }

      TEST_LOG_INFO {
        for (unsigned int i = 0; i < len; i++) {
          // log("'%c'",  *((char *)ptr + i));
          log("%c", *((char *) ptr + i));
        }
        log("\n");
      }

      return len;
    }
  }
  return 0;
}

template<class Type_A>
int SendMail<Type_A>::sendCUrlMail(const mailServerInfo& serverInfo, const mailHeader& header, const string& message, const string& filepath, const string& filename) {
  TRACEDEBUG_REGISTER("[CURL][MAIL]");
  LOG_ENTER();

  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx;

  upload_ctx.lines_read = 0;

  computeMessage(serverInfo.sender, header, message, filepath, filename);

  //Create file* to redirect curl error (not stderr)
  //we are stuck with C implementation because curl uses stderr (file) and not cerr (stream)
  FILE* fileLogs;
  fileLogs = fopen("/tmp/logsCurl", "w+");

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, serverInfo.serverUrl.c_str());

    LOG_DEBUG("smtp server : %s", serverInfo.serverUrl.c_str());

    if (serverInfo.authent) {
      /**@satisfies{EV_Parking_Board_CODE_67,EV_Parking_Board_CMPR_008_30,T} : authentication mode sending mail*/
      LOG_DEBUG("smtp user   : %s", serverInfo.user.c_str());
      LOG_DEBUG("smtp pwd    : %s", serverInfo.pwd.c_str());
      curl_easy_setopt(curl, CURLOPT_USERNAME, serverInfo.user.c_str());
      curl_easy_setopt(curl, CURLOPT_PASSWORD, serverInfo.pwd.c_str());
      curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
      curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
      // no cacert on target : disable server authetication:
      curl_easy_setopt(curl, CURLOPT_USE_SSL, (long ) CURLUSESSL_ALL);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
      curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
      //redirect logs to our file
      curl_easy_setopt(curl, CURLOPT_STDERR, fileLogs);
    }

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, serverInfo.sender.c_str());
#if 1
    //Broadcast to all receivers
    for (unsigned int i = 0; i < header.receivers.size(); i++) {
      LOG_DEBUG("add %s to CCI ", header.receivers.at(i).c_str());
      recipients = curl_slist_append(recipients, header.receivers.at(i).c_str());
    }
#else
    LOG_DEBUG("recipient : %s", header.receiver.c_str());
    recipients = curl_slist_append(recipients, header.receiver.c_str());
#endif

    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if (res != CURLE_OK) {
      LOG_ERROR("curl_easy_perform() failed: %s", curl_easy_strerror(res));
    }

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    curl_easy_cleanup(curl);

    //close the file because curl create issues with file access violation
    fclose(fileLogs);

    //now reopen it to parse and display...
    fileLogs = fopen("/tmp/logsCurl", "r");
    char buff[200];
    while (!feof(fileLogs)) {
      if (fgets(buff, 200, fileLogs) != NULL) {
        LOG_INFO("%s", strtok(buff, "\n"));
      } else if (ferror(fileLogs)) {
        LOG_ERROR("ERROR READING");
      }
    }
    fclose(fileLogs);

  } else {
    LOG_ERROR("bad curl statement");
  }

  return (int) res;
}

template<class Type_A>
bool SendMail<Type_A>::computeMessage(const string& sender, const mailHeader& header, const string& message, const string& filepath, const string& filename) {
  TRACEDEBUG_REGISTER("[CURL][MESSAGE]");
  LOG_ENTER();

  bool ret = true;

  payload_text.clear();
#if 1
  //Broadcasting alarms

  //If one receivers, send to him without hide receivers
  if (header.receivers.size() == 1) {
    payload_text.push_back("To: ");
    payload_text.push_back("<" + header.receivers.at(0) + ">");
  } else {
    payload_text.push_back("To: ");
    payload_text.push_back("<noreply@wallboxmail.com>");
  }
#else
  payload_text.push_back("To: ");
  payload_text.push_back("<" + header.receiver + ">");
#endif
  payload_text.push_back("\r\n");
  payload_text.push_back("From: ");
  payload_text.push_back("<" + sender + ">");
  payload_text.push_back("\r\n");
  payload_text.push_back("Subject: =?UTF-8?Q?" + qEncode(header.subject) + "?=");
  payload_text.push_back("\r\n");

  payload_text.push_back("MIME-Version: 1.0");
  payload_text.push_back("\r\n");
  payload_text.push_back("Content-Type: multipart/mixed; boundary=001a113cd4e8883bcd04fd99f266");
  payload_text.push_back("\r\n");

  payload_text.push_back("\r\n"); /* empty line to divide headers from body, see RFC5322 */
  payload_text.push_back("--001a113cd4e8883bcd04fd99f266\r\n");
  // first part is text content
  payload_text.push_back("Content-Type: text/plain; charset=UTF-8\r\n");
  payload_text.push_back("\r\n");
  payload_text.push_back(message + "\r\n");

  if (filename != "") {
    LOG_INFO("file %s in attachment", filename.c_str());
    // Read the entire file and line by line encode it
    string line;
    string filenameWithPath = filepath + filename;
    ifstream myfile(filenameWithPath.c_str());

    if (myfile.is_open()) {
      payload_text.push_back("--001a113cd4e8883bcd04fd99f266\r\n");
      payload_text.push_back("Content-Type: text/csv; charset=US-ASCII; name=\"" + filename + "\"\r\n");
      payload_text.push_back("Content-Disposition: attachment; filename=\"" + filename + "\"\r\n");
      payload_text.push_back("\r\n");
      while (getline(myfile, line)) {
        payload_text.push_back(line + "\r\n");
      }
      myfile.close();
    } else {
      LOG_ERROR("Can not open file %s", filenameWithPath.c_str());

      ret = false;
    }
    payload_text.push_back("\r\n");
  }
  payload_text.push_back("--001a113cd4e8883bcd04fd99f266--\r\n");
  payload_text.push_back("\r\n");
  return ret;
}

template<class Type_A>
string SendMail<Type_A>::qEncode(string stringToEncode) {
  /************************************************/
  /* According to RFC 2047 and RFC 2045 standards */
  /************************************************/

  string ret = "";
  char byte;
  const char hex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  for (unsigned int i = 0; i < stringToEncode.length(); ++i) {
    byte = stringToEncode[i];

    //All printable ascii char are not changed
    //Except '=', '?', and '_'
    if ((byte >= 33) && (byte <= 126) && (byte != 61) && (byte != 63) && (byte != 95)) {
      ret += byte;
    } else if (byte == 0x20) {
      //To be more readable, char 0x20 (white space) is replaced by '_'
      ret += '_';
    } else {
      //For all other char, add '=' in front of their two hexadecimal digits
      ret += '=';
      ret += hex[((byte >> 4) & 0x0F)];
      ret += hex[(byte & 0x0F)];
    }
  }
  return ret;
}

#endif /* SENDMAIL_H */
