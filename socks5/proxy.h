// proxy.h
#ifndef PROXY_H
#define PROXY_H

int receive_exact_data(int file_descriptor, void *buffer, size_t number_of_bytes, int flags);
int receive_string_data(int file_descriptor, char *string_buffer);
void send_server_hello_message(int file_descriptor, uint8_t method);
int handle_client_greeting(int client_file_descriptor);
void send_domain_reply_message(int file_descriptor, uint8_t reply_type, const char *domain, uint8_t domain_length, in_port_t port);
void send_ip_reply_message(int file_descriptor, uint8_t reply_type, in_addr_t ip_address, in_port_t port);
int handle_client_request(int client_file_descriptor);
void initiate_socks5_tunnel(int client_file_descriptor, int remote_file_descriptor);
void *client_thread_worker(void *arguments);
_Noreturn void run_server_loop(int server_file_descriptor);
void display_help_message(const char *program_name);
void *main_socks(void *arg);

#endif // PROXY_H
