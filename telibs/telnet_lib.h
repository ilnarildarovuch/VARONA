#ifndef SOCKET_LIB_H
#define SOCKET_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX_MSG_LENGTH 10240
#define TELNET_PORT 23
#define END_STRING "exit"

typedef void (*receive_callback)(const char* data, int length, void* user_data);

int send_cmd_programmatic(int sock, const char* cmd);
int receive_programmatic(int sock, char* buffer, int buffer_size);
void set_receive_callback(receive_callback callback, void* user_data);

void send_cmd(int sock, int pid);
void receive(int sock);

#endif // SOCKET_LIB_H