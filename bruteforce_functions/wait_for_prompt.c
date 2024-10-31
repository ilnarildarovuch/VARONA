int wait_for_prompt(int sock) {
    char buffer[TELNET_MAX_BUFFER];
    int bytes_received;
    time_t start_time = time(NULL);

    while (1) {
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            if (strstr(buffer, ":")) {
                printf("DEBUG: Найдено приглашение для ввода: %s\n", buffer);
                return 1; // Найдено ": "
            }
        }
        // Проверка времени ожидания
        if (difftime(time(NULL), start_time) > 10) {
            printf("DEBUG: Время ожидания истекло\n");
            return 0; // Время ожидания истекло
        }
    }
}