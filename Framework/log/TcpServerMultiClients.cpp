#include "TcpServerMultiClients.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

TcpServerMultiClients::TcpServerMultiClients(int port) :
    TcpServer(port) {
}

int TcpServerMultiClients::ReceivedFromMultipleClients(void (*dataFormater)(char * str)) {
  // 'accept' system call parameters
  struct sockaddr_in clientaddr;
  socklen_t addrlen = sizeof(clientaddr);

  // 'select' system call parameters 
  fd_set master; /* master file descriptor list */
  fd_set read_fds; /* temp file descriptor list for select() */
  int listener; /* listening socket descriptor */
  int fdmax; /* maximum file descriptor number */
  int newfd; /* newly accept()ed socket descriptor */

  /* buffer for client data */
  char buf[1024];
  int nbytes;

  /* clear the master and temp sets */
  FD_ZERO(&master);
  FD_ZERO(&read_fds);

  /* Create server socket */
  if (!_list_s) {
    CreateSocketServer();
  }

  // set the socket server as the listener
  listener = _list_s;

  /* add the listener to the master set */
  FD_SET(listener, &master);

  /* keep track of the biggest file descriptor */
  fdmax = listener; /* so far, it's this one*/

  /* loop */
  for (;;) {
    /* copy it */
    read_fds = master;

    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("Server-select() error lol!");
      exit(1);
    }

    // printf("Server-select() is OK...\n");

    /* run through the existing connections looking for data to be read*/
    for (int curfd = 0; curfd <= fdmax; curfd++) {
      if (FD_ISSET(curfd, &read_fds)) {
        /* we got one... */
        if (curfd == listener) {
          /* handle new connections */
          addrlen = sizeof(clientaddr);
          if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &addrlen)) == -1) {
            perror("Server-accept() error lol!");
          } else {
            // printf("Server-accept() is OK...\n");
            FD_SET(newfd, &master); /* add to master set */

            if (newfd > fdmax) { /* keep track of the maximum */
              fdmax = newfd;
            }
            printf("<!--New connection from %s on socket %d-->\n", inet_ntoa(clientaddr.sin_addr),
                   newfd);

            // save ip addr to table
            strncpy(_clientAddr[newfd], inet_ntoa(clientaddr.sin_addr), CLIENT_IP_STR_SIZE);

          }
        } else {
          /* handle data from a client */
          if ((nbytes = recv(curfd, buf, sizeof(buf), 0)) <= 0) {
            /* got error or connection closed by client */
            if (nbytes == 0) {
              /* connection closed */
              printf("<!--socket %d @%s hung up-->\n", curfd, _clientAddr[curfd]);
            } else {
              perror("recv() error lol!");
            }

            /* close it... */
            close(curfd);

            /* remove from master set */
            FD_CLR(curfd, &master);

          } else {
            // force '\0' at the end of received string
            buf[nbytes] = '\0';

            // compute header
            snprintf(_header, HEADER_SIZE, "\n[%d@%s]\n", curfd, _clientAddr[curfd]);

            // print if different as previous header
            if (strcmp(_header, _lastHeader)) {
              dataFormater(_header);
              snprintf(_lastHeader, HEADER_SIZE, "%s", _header);
            }

            dataFormater(buf);

          }
        }
      }
    }
  }
}
