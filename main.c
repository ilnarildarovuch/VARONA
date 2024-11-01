#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "include/includes.h"

int main() {
    Credentials ssh_credentials = ssh_brute(SSH_HOST);
    if (ssh_credentials.good) {printf("Успешный вход: %s:%s\n", ssh_credentials.username, ssh_credentials.password);}
    else {printf("Не удалось войти на %s\n", SSH_HOST);}

    Credentials telnet_credentials = telnet_brute(TELNET_HOST);
    if (telnet_credentials.good) {printf("Успешный вход: %s:%s\n", telnet_credentials.username, telnet_credentials.password);}
    else {printf("Не удалось войти на %s\n", TELNET_HOST);}
    return 0;
}