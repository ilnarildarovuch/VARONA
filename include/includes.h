#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <libssh/libssh.h>

#include "credentials.h"

#define TELNET_PORT 23
#define SSH_PORT 22

#define TELNET_MAX_BUFFER 10240
#define TELNET_HOST "192.168.1.1"
#define SSH_HOST "127.0.0.1"