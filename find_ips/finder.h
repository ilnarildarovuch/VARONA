#ifndef FINDER_H
#define FINDER_H

void generate_random_ip(char *ip_address, size_t length);
int check_port_open_on_ip(const char *ip_address, int port);

#endif // FINDER_H