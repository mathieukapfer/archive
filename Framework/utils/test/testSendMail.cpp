/* 
 * File:   testSendMail.cpp
 * Author: romain
 *
 * Created on 11 avril 2014, 09:48
 */

#include <cstdlib>
#include "SendMail.h"
using namespace std;

/*
 * 
 */
int main(void) {
  SendMail<dummy1> mailSender;

  mailServerInfo server;

  server.authent = true;

  server.serverUrl = "smtps://wallboxmail.com:587";
  server.user = "idy";
  server.pwd = "ydi";
  server.sender = "service@wallboxmail.com";
  mailHeader header;
  header.receiver = "mysupermailtester@gmail.com";
  header.subject = "=You are the one I lové_?=alloé Загрузочные данные";

  string message = "Ouh ouh ouh ";

  mailSender.sendCUrlMail(server, header, message, "", "");

}

