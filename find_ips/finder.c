#include "include/includes.h"
#include "finder.h"

void generate_random_ip(char *ip_address, size_t length) {
    // Генерация первого октета, исключая 127
    int part1, part2, part3, part4;

     do {
        part1 = rand() % 256;
    } while (part1 == 0 ||        // 0.0.0.0/8
            part1 == 10 ||       // 10.0.0.0/8
            part1 == 127 ||      // 127.0.0.0/8
            part1 == 169 ||      // 169.254.0.0/16
            part1 == 172 ||      // 172.16.0.0/12
            part1 == 192 ||      // 192.168.0.0/16
            part1 == 224 ||      // 224.0.0.0/4 (multicast)
            part1 == 240 ||      // 240.0.0.0/4 (reserved)
            part1 == 255);

    // Генерация остальных октетов
    part2 = rand() % 256;
    part3 = rand() % 256;
    part4 = rand() % 256;

    // Форматирование строки IP адреса
    snprintf(ip_address, length, "%d.%d.%d.%d", part1, part2, part3, part4);
}

int check_port_open_on_ip(const char *ip_address, int port) {
    int sock;
    struct sockaddr_in server;

    // Создаем сокет
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        return -1;
    }

    // Настраиваем структуру sockaddr_in
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &server.sin_addr) <= 0) {
        close(sock);
        return -1;
    }

    // Устанавливаем таймаут для сокета
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
        close(sock);
        return -1;
    }

    // Пытаемся подключиться к серверу
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        close(sock);
        if (errno == EINPROGRESS) {
            // Порт закрыт или недоступен
            return 0; // Порт закрыт
        }
        return 0; // Порт закрыт
    }

    close(sock);
    return 1; // Порт открыт
}