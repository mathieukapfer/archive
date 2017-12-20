/* 
 * File:   PingUtils.cpp
 * Author: romain
 * 
 * Created on 1 juillet 2014, 15:13
 */

#include "PingUtils.h"
#include <cstdlib>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <cstdio>

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <netdb.h>

#include <unistd.h>

extern "C" {
// wait pid    
#include <sys/wait.h>
}

#include "TraceDebug.h"

#define PING_TIMEOUT_IN_SEC "3"

TRACEDEBUG_REGISTER("[UTILS][PING]");

using namespace std;

PingUtils::PingUtils() {
  LOG_ENTER_();
}

PingUtils::~PingUtils() {
  LOG_ENTER();
}
#if 0
// used from http://stackoverflow.com/questions/8290046/icmp-sockets-linux
//note, to allow root to use icmp sockets, run:
//sysctl -w net.ipv4.ping_group_range="0 0"

bool PingUtils::ping(char *AdresseAPinger) {
  bool ret = false;
  struct in_addr dst;

  if (inet_aton(AdresseAPinger, &dst) == 0) {
    LOG_ERROR("%s isn't a valid IP address", AdresseAPinger);
    ret = false;
  } else {
    struct icmphdr icmp_hdr;
    struct sockaddr_in addr;
    int sequence = 0;
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_ICMP);
    if (sock < 0) {
      LOG_DEBUG("KO : socket creation");
      ret = false;
    } else {
      //memset(&addr, 0, sizeof addr);
      addr.sin_family = AF_INET;
      addr.sin_addr = dst;

      //memset(&icmp_hdr, 0, sizeof icmp_hdr);
      icmp_hdr.type = ICMP_ECHO;
      icmp_hdr.un.echo.id = 1234;  //arbitrary id

      unsigned char data[2048];
      int rc;
      struct timeval timeout = { 3, 0 };  //wait max 3 seconds for a reply
      fd_set read_set;
      socklen_t slen;
      struct icmphdr rcv_hdr;

      icmp_hdr.un.echo.sequence = sequence++;
      memcpy(data, &icmp_hdr, sizeof icmp_hdr);
      memcpy(data + sizeof icmp_hdr, "hello", 5);  //icmp payload
      rc = sendto(sock, data, sizeof icmp_hdr + 5, 0, (struct sockaddr*) &addr, sizeof addr);
      if (rc <= 0) {
        LOG_DEBUG("KO : sendTo");
        ret = false;
      } else {
        LOG_DEBUG("send Icmp");

        memset(&read_set, 0, sizeof read_set);
        FD_SET(sock, &read_set);

        //wait for a reply with a timeout
        rc = select(sock + 1, &read_set, NULL, NULL, &timeout);
        if (rc == 0) {
          LOG_DEBUG("KO : no reply");
          ret = false;
        } else if (rc < 0) {
          LOG_DEBUG("KO : select");
          ret = false;
        } else {
          //we don't care about the sender address in this example..
          slen = 0;
          rc = recvfrom(sock, data, sizeof data, 0, NULL, &slen);
          if (rc <= 0) {
            LOG_DEBUG("KO : recvfrom");
            ret = false;
          } else if (rc < sizeof rcv_hdr) {
            LOG_DEBUG("KO : Error, got short ICMP packet, %d bytes", rc);
            ret = false;
          } else {
            memcpy(&rcv_hdr, data, sizeof rcv_hdr);
            if (rcv_hdr.type == ICMP_ECHOREPLY) {
              LOG_DEBUG("OK : ICMP Reply, id=0x%x, sequence =  0x%x", icmp_hdr.un.echo.id, icmp_hdr.un.echo.sequence);
              ret = true;
            } else {
              LOG_DEBUG("KO : Got ICMP packet with type 0x%x ?!?", rcv_hdr.type);
            }
          }
        }
      }
    }
  }
  return ret;
}
#else
bool PingUtils::ping(char* AddrToPing) {
  LOG_ENTER();
  bool ret = false;
  fflush(stdout);
  fflush(stderr);
  int pid = fork();
  if (pid == 0) {  // child
    LOG_DEBUG("ping %s ", AddrToPing);
    // 1) prepare the argv to give to the program
    char *argv[] = {"/bin/ping", AddrToPing, "-c", "1", "-w",
      PING_TIMEOUT_IN_SEC,
      NULL};
    LOG_DEBUG("CHILD : Execv ping (PID = %d PPID = %d) ", getpid(), getppid());
    // 3) exec the ntpd command
    execv("/bin/ping", argv);
    LOG_INFO("Error: command ping cannot be launched");

    _exit(1);// exit with error
  } else if (pid < 0) {  // fork failed
    LOG_INFO("fork() failed<br/>");
    _exit(1);
  } else {
    // father: wait for the child and report it's success/failure
    int status = 0;
    LOG_DEBUG("Father (pid = %d ) waiting for child (pid = %d) ", getpid(), pid);
    waitpid(pid, &status, 0);
    LOG_DEBUG("Father : end wait");
    ret = (WIFEXITED(status) && (WEXITSTATUS(status)) == 0);
  }
  LOG_EXIT("%s", ret ?"True":"False");
  return ret;
}
#endif
