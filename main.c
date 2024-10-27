#include "telibs/telnet_lib.h"
#include "telnet_check.c"

void handle_login(const char* data, int length, void* user_data) {
    int* sock = (int*)user_data;

    printf("%s", data);

    if (strstr(data, ":") != NULL) {
        static int login_state = 0;

        if (login_state == 0) {
            send_cmd_programmatic(*sock, "admin");
            login_state = 1;
        } else if (login_state == 1) {
            send_cmd_programmatic(*sock, "admin");
            login_state = 2;
        }
    }
}

int main(int argc, char **argv) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    telnet_chk(sock, argc, argv);
    
    set_receive_callback(handle_login, &sock);
    
    char recv_buffer[MAX_MSG_LENGTH];
    int received;
    
    while (1) {
        received = receive_programmatic(sock, recv_buffer, MAX_MSG_LENGTH);
        if (received <= 0) {
            printf("Connection closed or error\n");
            break;
        }
        
        if (strstr(recv_buffer, "#") != NULL || strstr(recv_buffer, "$") != NULL || strstr(recv_buffer, "(config)") != NULL) {
            printf("Login successful! You can now send commands.\n");
            
            char cmd[MAX_MSG_LENGTH];
            
            while (fgets(cmd, MAX_MSG_LENGTH, stdin)) {
                if (strncmp(cmd, END_STRING, strlen(END_STRING)) == 0) {
                    break;
                }
                send_cmd_programmatic(sock, cmd);
                
                received = receive_programmatic(sock, recv_buffer, MAX_MSG_LENGTH);
                if (received <= 0) break;
                
                printf("%s", recv_buffer);
                
            }
            break;
        }
    }
    
    close(sock);
    return 0;
}