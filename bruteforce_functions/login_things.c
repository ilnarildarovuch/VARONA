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
    int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        perror("Ошибка при получении данных");
        return 0;
    }
    buffer[bytes_received] = '\0';
    printf("DEBUG: Полученный ответ: %s\n", buffer); // Отладочное сообщение

    // Проверка на успешный вход
    if (strstr(buffer, "Login") || strstr(buffer, "Password")) {
        printf("DEBUG: Сервер все еще ожидает входа.\n");
        return 0; // Сервер все еще ожидает входа
    }

    if (strstr(buffer, "#") || strstr(buffer, "$") || strstr(buffer, "(config)") ||
        strstr(buffer, "/ ]") || strstr(buffer, "\\ ]")) {
        return 1; // Успешный вход
    } else {
        printf("DEBUG: Неудачная попытка входа. Ответ сервера не соответствует ожиданиям.\n");
        return 0; // Неудачный вход
    }
}
