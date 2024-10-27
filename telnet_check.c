void telnet_chk(int sock, int argc, char **argv)
{

    if (sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (argc != 2) {
        perror("args");
        exit(EXIT_FAILURE);
    }

    struct in_addr server_addr;
    if (!inet_aton(argv[1], &server_addr)) {
        perror("inet_aton");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in connection;
    connection.sin_family = AF_INET;
    memcpy(&connection.sin_addr, &server_addr, sizeof(server_addr));
    connection.sin_port = htons(TELNET_PORT);
    if (connect(sock, (const struct sockaddr*) &connection, sizeof(connection)) != 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
}