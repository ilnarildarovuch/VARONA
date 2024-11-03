#include "include/includes.h"
#include "proxy.h"

typedef struct {
    uint8_t version;
    uint8_t num_methods;
} socks5_client_hello_t;

typedef struct {
    uint8_t version;
    uint8_t method;
} socks5_server_hello_t;

typedef struct {
    uint8_t version;
    uint8_t command;
    uint8_t reserved;
    uint8_t addr_type;
} socks5_request_t;

typedef struct {
    uint8_t version;
    uint8_t reply;
    uint8_t reserved;
    uint8_t addr_type;
} socks5_reply_t;

int receive_exact_data(int file_descriptor, void *buffer, size_t number_of_bytes, int flags) {
    size_t remaining_bytes = number_of_bytes;
    while (remaining_bytes > 0) {
        ssize_t received_length = recv(file_descriptor, buffer, remaining_bytes, flags);
        if (received_length < 0) {
            if (errno == EINTR || errno == EAGAIN) {
                // EINTR: interrupted by system
                // EAGAIN: recv is blocked
                // Try again
                continue;
            } else {
                // Unexpected error
                perror("recv()");
                return -1;
            }
        } else if (received_length == 0) {
            // No data from socket: disconnected.
            return -1;
        } else {
            // read success
            remaining_bytes -= received_length;
            buffer += received_length;
        }
    }
    return 0;
}

int receive_string_data(int file_descriptor, char *string_buffer) {
    uint8_t length;
    if (receive_exact_data(file_descriptor, &length, sizeof(uint8_t), 0) != 0) {
        return -1;
    }
    if (receive_exact_data(file_descriptor, string_buffer, length, 0) != 0) {
        return -1;
    }
    string_buffer[length] = '\0';
    return length;
}

void send_server_hello_message(int file_descriptor, uint8_t method) {
    socks5_server_hello_t server_hello_message = {
            .version = SOCKS5_VERSION,
            .method = method,
    };
    send(file_descriptor, &server_hello_message, sizeof(socks5_server_hello_t), 0);
}

int handle_client_greeting(int client_file_descriptor) {
    socks5_client_hello_t client_hello_message;
    if (receive_exact_data(client_file_descriptor, &client_hello_message, sizeof(socks5_client_hello_t), 0) != 0) {
        return -1;
    }

    if (client_hello_message.version != SOCKS5_VERSION) {
        fprintf(stderr, "Unsupported socks version %#02x\n", client_hello_message.version);
        return -1;
    }
    uint8_t methods[UINT8_MAX];
    if (receive_exact_data(client_file_descriptor, methods, client_hello_message.num_methods, 0) != 0) {
        return -1;
    }
    // Find server auth method in client's list
    int found_method = 0;
    for (int i = 0; i < (int) client_hello_message.num_methods; i++) {
        if (methods[i] == SOCKS5_AUTH_NO_AUTH) {
            // Find auth method in client's supported method list
            found_method = 1;
            break;
        }
    }
    if (!found_method) {
        // No acceptable method
        fprintf(stderr, "No acceptable method from client\n");
        send_server_hello_message(client_file_descriptor, SOCKS5_AUTH_NOT_ACCEPT);
        return -1;
    }
    // Send auth method choice
    send_server_hello_message(client_file_descriptor, SOCKS5_AUTH_NO_AUTH);
    return 0;
}

void send_domain_reply_message(int file_descriptor, uint8_t reply_type, const char *domain, uint8_t domain_length, in_port_t port) {
    uint8_t buffer[sizeof(uint8_t) + UINT8_MAX + sizeof(in_port_t)];
    uint8_t *pointer = buffer;
    *(socks5_reply_t *) pointer = (socks5_reply_t) {
            .version = SOCKS5_VERSION,
            .reply = reply_type,
            .reserved = 0,
            .addr_type = SOCKS5_ATYP_DOMAIN_NAME
    };
    pointer += sizeof(socks5_reply_t);
    *pointer = domain_length;
    pointer += sizeof(uint8_t);
    memcpy(pointer, domain, domain_length);
    pointer += domain_length;
    *(in_port_t *) pointer = port;
    pointer += sizeof(in_port_t);
    send(file_descriptor, buffer, pointer - buffer, 0);
}

void send_ip_reply_message(int file_descriptor, uint8_t reply_type, in_addr_t ip_address, in_port_t port) {
    uint8_t buffer[sizeof(socks5_reply_t) + sizeof(in_addr_t) + sizeof(in_port_t)];
    uint8_t *pointer = buffer;
    *(socks5_reply_t *) pointer = (socks5_reply_t) {
        .version = SOCKS5_VERSION,
            .reply = reply_type,
            .reserved = 0,
            .addr_type = SOCKS5_ATYP_IPV4
    };
    pointer += sizeof(socks5_reply_t);
    *(in_addr_t *) pointer = ip_address;
    pointer += sizeof(in_addr_t);
    *(in_port_t *) pointer = port;
    send(file_descriptor, buffer, sizeof(buffer), 0);
}

int handle_client_request(int client_file_descriptor) {
    // Handle socks request
    socks5_request_t request;
    if (receive_exact_data(client_file_descriptor, &request, sizeof(socks5_request_t), 0) != 0) {
        return -1;
    }

    if (request.version != SOCKS5_VERSION) {
        fprintf(stderr, "Unsupported socks version %#02x\n", request.version);
        return -1;
    }
    if (request.command != SOCKS5_CMD_CONNECT) {
        fprintf(stderr, "Unsupported command %#02x\n", request.command);
        return -1;
    }

    int remote_file_descriptor = -1;
    if (request.addr_type == SOCKS5_ATYP_IPV4) {
        in_addr_t ip_address;
        if (receive_exact_data(client_file_descriptor, &ip_address, sizeof(in_addr_t), 0) != 0) {
            return -1;
        }
        in_port_t port;
        if (receive_exact_data(client_file_descriptor, &port, sizeof(in_port_t), 0) != 0) {
            return -1;
        }

        struct sockaddr_in remote_address;
        remote_address.sin_family = AF_INET;
        remote_address.sin_addr.s_addr = ip_address;
        remote_address.sin_port = port;

        remote_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (remote_file_descriptor < 0) {
            perror("socket()");
            send_ip_reply_message(client_file_descriptor, SOCKS5_REP_GENERAL_FAILURE, ip_address, port);
            return -1;
        }
        if (connect(remote_file_descriptor, (struct sockaddr *) &remote_address, sizeof(remote_address)) < 0) {
            perror("connect()");
            close(remote_file_descriptor);
            send_ip_reply_message(client_file_descriptor, SOCKS5_REP_GENERAL_FAILURE, ip_address, port);
            return -1;
        }
        printf("Connected to remote address %s:%d with FD %d\n",
               inet_ntoa(remote_address.sin_addr), ntohs(port), remote_file_descriptor);

        send_ip_reply_message(client_file_descriptor, SOCKS5_REP_SUCCESS, ip_address, port);
    } else if (request.addr_type == SOCKS5_ATYP_DOMAIN_NAME) {
        // Get domain name
        char domain[UINT8_MAX + 1];
        int domain_length = receive_string_data(client_file_descriptor, domain);
        if (domain_length <= 0) {
            return -1;
        }
        // Get port
        in_port_t port;
        if (receive_exact_data(client_file_descriptor, &port, sizeof(in_port_t), 0) != 0) {
            return -1;
        }

        // Get ip by host name
        char port_string[8];
        sprintf(port_string, "%d", ntohs(port));
        struct addrinfo *address_info;
        if (getaddrinfo(domain, port_string, NULL, &address_info) != 0) {
            perror("getaddrinfo()");
            send_domain_reply_message(client_file_descriptor, SOCKS5_REP_GENERAL_FAILURE, domain, domain_length, port);
            return -1;
        }
        // Try connecting to host
        for (struct addrinfo *ai = address_info; ai != NULL; ai = ai->ai_next) {
            int try_file_descriptor = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
            if (try_file_descriptor == -1) { continue; }
            if (connect(try_file_descriptor, ai->ai_addr, ai->ai_addrlen) == 0) {
                remote_file_descriptor = try_file_descriptor;
                break;
            } else {
                close(try_file_descriptor);
            }
        }
        freeaddrinfo(address_info);

        if (remote_file_descriptor == -1) {
            fprintf(stderr, "Cannot connect to remote address %s:%d\n", domain, ntohs(port));
            send_domain_reply_message(client_file_descriptor, SOCKS5_REP_GENERAL_FAILURE, domain, domain_length, port);
            return -1;
        }
        printf("Connected to remote address %s:%d with FD %d\n", domain, ntohs(port), remote_file_descriptor);

        send_domain_reply_message(client_file_descriptor, SOCKS5_REP_SUCCESS, domain, domain_length, port);
    } else {
        fprintf(stderr, "Unsupported address type %#02x\n", request.addr_type);
        return -1;
    }
    return remote_file_descriptor;
}

void initiate_socks5_tunnel(int client_file_descriptor, int remote_file_descriptor) {
    printf("Running SOCKS5 tunnel between FD %d and %d\n", client_file_descriptor, remote_file_descriptor);

    int maximum_file_descriptor = (client_file_descriptor > remote_file_descriptor) ? client_file_descriptor : remote_file_descriptor;
    uint8_t buffer[BUFSIZ];

    while (1) {
        fd_set read_set;
        FD_ZERO(&read_set);
        FD_SET(client_file_descriptor, &read_set);
        FD_SET(remote_file_descriptor, &read_set);

        if (select(maximum_file_descriptor + 1, &read_set, NULL, NULL, NULL) < 0) {
            perror("select()");
            continue;
        }

        if (FD_ISSET(client_file_descriptor, &read_set)) {
            ssize_t length = recv(client_file_descriptor, buffer, BUFSIZ, 0);
            if (length <= 0) { break; }
            send(remote_file_descriptor, buffer, length, 0);
        }

        if (FD_ISSET(remote_file_descriptor, &read_set)) {
            ssize_t length = recv(remote_file_descriptor, buffer, BUFSIZ, 0);
            if (length <= 0) { break; }
            send(client_file_descriptor, buffer, length, 0);
        }
    }
}

void *client_thread_worker(void *arguments) {
    int client_file_descriptor = (intptr_t) arguments;
    if (handle_client_greeting(client_file_descriptor) != 0) {
        close(client_file_descriptor);
        return NULL;
    }
    int remote_file_descriptor = handle_client_request(client_file_descriptor);
    if (remote_file_descriptor == -1) {
        close(client_file_descriptor);
        return NULL;
    }
    initiate_socks5_tunnel(client_file_descriptor, remote_file_descriptor);
    close(remote_file_descriptor);
    close(client_file_descriptor);
    return NULL;
}

_Noreturn void run_server_loop(int server_file_descriptor) {
    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_length = sizeof(client_address);

        // Check for incoming connections
        int client_file_descriptor = accept(server_file_descriptor, (struct sockaddr *) &client_address, &client_address_length);
        if (client_file_descriptor < 0) {
            perror("accept()");
            continue;
        }
        // Disable Nagle algorithm to forward packets ASAP
        int option_value = 1;
        if (setsockopt(client_file_descriptor, SOL_TCP, TCP_NODELAY, &option_value, sizeof(option_value)) < 0) {
            perror("setsockopt()");
            close(client_file_descriptor);
            continue;
        }

        printf("Accepted connection from %s:%d with FD %d\n",
               inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), client_file_descriptor);

        pthread_t client_thread_id;
        if (pthread_create(&client_thread_id, NULL, &client_thread_worker, (void *) (intptr_t) client_file_descriptor) == 0) {
            pthread_detach(client_thread_id);
        } else {
            perror("pthread_create()");
            close(client_file_descriptor);
        }
    }
}

void display_help_message(const char *program_name) {
    printf("USAGE: %s [-h] [-p PORT]\n", program_name);
}

void main_socks() {
    int bind_port = 5544;

    // Create a socket using TCP protocol over IPv4
    int server_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_file_descriptor < 0) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }
    // Reuse address
    int option_value = 1;
    if (setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) < 0) {
        perror("setsockopt()");
        close(server_file_descriptor);
        exit(EXIT_FAILURE);
    }
    // Bind socket to given address
    struct sockaddr_in bind_address;
    bind_address.sin_family = AF_INET;
    bind_address.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_address.sin_port = htons(bind_port);
    if (bind(server_file_descriptor, (struct sockaddr *) &bind_address, sizeof(bind_address)) < 0) {
        perror("bind()");
        close(server_file_descriptor);
        exit(EXIT_FAILURE);
    }
    // Listen to socket
    if (listen(server_file_descriptor, SOMAXCONN) < 0) {
        perror("listen()");
        close(server_file_descriptor);
        exit(EXIT_FAILURE);
    }
    printf("Server listening on %s:%d\n", inet_ntoa(bind_address.sin_addr), bind_port);
    // Run server
    run_server_loop(server_file_descriptor);
}
