char* close_socket(int sock, char* return_value) {
    close(sock);
    return return_value;
}

int create_socket(const char *host, struct sockaddr_in *server) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return -1;

    server->sin_family = AF_INET;
    server->sin_port = htons(TELNET_PORT);
    inet_pton(AF_INET, host, &server->sin_addr);
    return connect(sock, (struct sockaddr *)server, sizeof(*server)) < 0 ? -1 : sock;
}