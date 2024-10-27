#ifndef SOCKET_LIB_H
#define SOCKET_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_MSG_LENGTH 1024
#define TELNET_PORT 23
#define END_STRING "exit"

void send_cmd(int sock, int pid);
void receive(int sock);

#endif // SOCKET_LIB_H
