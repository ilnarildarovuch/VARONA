void send_username(int sock, const char *username) {
    for (const char *c = username; *c; c++) {
        send(sock, c, 1, 0);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 90000000; // 0.09 seconds in nanoseconds
        nanosleep(&ts, NULL);
    }
    send(sock, "\n", 1, 0);
}

void send_password(int sock, const char *password) {
    for (const char *c = password; *c; c++) {
        send(sock, c, 1, 0);
        struct timespec ts;
        ts.tv_sec = 0;
        ts.tv_nsec = 90000000; // 0.09 seconds in nanoseconds
        nanosleep(&ts, NULL);
    }
    send(sock, "\n", 1, 0);
}

int check_login_success(int sock, char *buffer) {
    int bytes_received = recv(sock, buffer, TELNET_MAX_BUFFER - 1, 0); // Исправлено использование размера буфера
    if (bytes_received < 0) {
        return 0;
    }
    buffer[bytes_received] = '\0';
    printf("DEBUG: Полученный ответ: %s\n", buffer);

    // Проверка на неудачный вход
    if (strstr(buffer, "Login") || strstr(buffer, "Password") || 
        strstr(buffer, "failed") || strstr(buffer, "incorrect")) {
        printf("DEBUG: Неудачная попытка входа\n");
        return 0;
    }

    // Расширенная проверка на успешный вход
    if (strstr(buffer, "#") || strstr(buffer, "$") || strstr(buffer, ">") ||
        strstr(buffer, "(config)") || strstr(buffer, "/ ]") || strstr(buffer, "\\ ]") ||
        strstr(buffer, "Menu") || strstr(buffer, "Welcome")) {
        return 1;
    }

    return 0;
}
