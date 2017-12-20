#include <stdio.h>
#include "TcpServerMultiClients.h"

void dataFormater(char * str) {
  printf(" %s", str);
}

int main() {
  TcpServerMultiClients tcpServer(1501);

  while (1) {
    tcpServer.ReceivedFromMultipleClients(dataFormater);
  }

}
