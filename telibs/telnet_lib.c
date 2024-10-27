#include "telnet_lib.h"


void send_cmd(int sock, int pid) {
    char str[MAX_MSG_LENGTH] = {0};
    printf("> ");
    while (fgets(str, MAX_MSG_LENGTH, stdin) == str) {
        if (strncmp(str, END_STRING, strlen(END_STRING)) == 0) break;
        if (send(sock, str, strlen(str) + 1, 0) < 0) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }
    kill(pid, SIGKILL);
    printf("Goodbye.\n");
}

void receive(int sock) {
    char buf[MAX_MSG_LENGTH] = {0};
    int filled = 0;    
    while ((filled = recv(sock, buf, MAX_MSG_LENGTH - 1, 0)) > 0) {
        buf[filled] = '\0';
        printf("%s", buf);
        fflush(stdout);        
    }    
    printf("Server disconnected.\n");
}
