#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define TELNET_PORT 23
#define TELNET_MAX_BUFFER 10240
#define HOST "192.168.1.1"