#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <netdb.h>
#include <sys/socket.h>
#include <libssh/libssh.h>
#include <netinet/tcp.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>
#include <setjmp.h>

#include "credentials.h"

#define TELNET_PORT 23
#define SSH_PORT 22

#define IP_ADDRESS_LENGTH 16
#define TELNET_MAX_BUFFER 10240
#define TELNET_HOST "192.168.1.1"
#define SSH_HOST "127.0.0.1"

// Version
#define SOCKS5_VERSION 0x05

// Authentication
#define SOCKS5_AUTH_NO_AUTH 0x00
#define SOCKS5_AUTH_NOT_ACCEPT 0xff

// Command
#define SOCKS5_CMD_CONNECT 0x01
#define SOCKS5_CMD_BIND 0x02
#define SOCKS5_CMD_ASSOCIATE 0x03

// Address type
#define SOCKS5_ATYP_IPV4 0x01
#define SOCKS5_ATYP_DOMAIN_NAME 0x03
#define SOCKS5_ATYP_IPV6 0x04

// Reply
#define SOCKS5_REP_SUCCESS 0x00
#define SOCKS5_REP_GENERAL_FAILURE 0x01
#define SOCKS5_REP_CONNECTION_NOT_ALLOWED 0x02
#define SOCKS5_REP_NET_UNREACHABLE 0x03
#define SOCKS5_REP_HOST_UNREACHABLE 0x04
#define SOCKS5_REP_CONNECTION_REFUSED 0x05
#define SOCKS5_REP_TTL_EXPIRED 0x06
#define SOCKS5_REP_COMMAND_NOT_SUPPORTED 0x07
#define SOCKS5_REP_ADDR_TYPE_NOT_SUPPORTED 0x08