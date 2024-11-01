#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <libssh/libssh.h>

#define TELNET_PORT 23
#define SSH_PORT 22

#define TELNET_MAX_BUFFER 10240
#define HOST "192.168.1.1"

const char *usernames[] = {
    "root", "admin", "root", "support", "user", "tech", "mother", "superadmin",
    "admin1234", "1111111", "default", "", NULL
};

const char *passwords[] = {
    "admin", "klv123", "ikwb", "admin1234", "1111111", "Zte521", "12345",
    "123456", "anko", "666666", "realtek", "root", "pass", "password",
    "jvbzd", "xc3511", "1234", "system", "dreambox", "smcadmin", "klv1234",
    "zlxx.", "tech", "fucker", "00000000", "supervisor", "vizxv", "1111",
    "default", "guest", "meinsm", "xmhdipc", "54321", "service", "888888",
    "juantech", "user", "hi3518", "support", "7ujMko0admin", "7ujMko0vizxv",
    "superadmin", "", NULL
};