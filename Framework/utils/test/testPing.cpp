/* 
 * File:   testPing.cpp
 * Author: romain
 *
 * Created on 1 juillet 2014, 15:22
 */

#include <cstdlib>
#include "PingUtils.h"
#include <cstdio>
using namespace std;

int main(int arc, char *argv[]) {
  PingUtils ping;

  printf("test 192.168.0.1 = %s\n", ping.ping("192.168.0.1") ? "true" : "false");
  printf("test 192.168.0.11 = %s\n", ping.ping("192.168.0.11") ? "true" : "false");

  return 0;
}
