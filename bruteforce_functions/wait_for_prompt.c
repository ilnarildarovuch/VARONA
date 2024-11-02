int wait_for_prompt(int sock) {
    char buffer[TELNET_MAX_BUFFER];
    int bytes_received;
    time_t start_time = time(NULL);

    while (1) {
        bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("DEBUG: Получено: %s\n", buffer);
            
            // Расширенная проверка приглашений
            if (strstr(buffer, ":") || strstr(buffer, "login") || 
                strstr(buffer, "password") || strstr(buffer, ">") ||
                strstr(buffer, "@")) {
                return 1;
            }
        }
        
        if (difftime(time(NULL), start_time) > 15) { // Увеличено время ожидания
            printf("DEBUG: Время ожидания истекло\n");
            return 0;
        }
        
        // Добавляем небольшую задержку между попытками
        usleep(100000); // 0.1 секунды
    }
}