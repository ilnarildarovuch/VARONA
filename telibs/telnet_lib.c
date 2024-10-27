#include "telnet_lib.h"

static receive_callback g_callback = NULL;
static void* g_user_data = NULL;

void set_receive_callback(receive_callback callback, void* user_data) {
    g_callback = callback;
    g_user_data = user_data;
}

int send_cmd_programmatic(int sock, const char* cmd) {
    if (!cmd) return -1;
    
    size_t cmd_len = strlen(cmd);
    if (cmd_len > MAX_MSG_LENGTH - 1) return -1;
    
    char formatted_cmd[MAX_MSG_LENGTH];
    strncpy(formatted_cmd, cmd, MAX_MSG_LENGTH - 2);
    
    if (cmd_len > 0 && cmd[cmd_len-1] != '\n') {
        formatted_cmd[cmd_len] = '\n';
        formatted_cmd[cmd_len + 1] = '\0';
        cmd_len++;
    }

    int sent = send(sock, formatted_cmd, cmd_len + 1, 0);
    if (sent < 0) {
        return -1;
    }
    
    return sent;
}

int receive_programmatic(int sock, char* buffer, int buffer_size) {
    if (!buffer || buffer_size <= 0) return -1;
    
    int filled = recv(sock, buffer, buffer_size - 1, 0);
    if (filled > 0) {
        buffer[filled] = '\0';
        
        if (g_callback) {
            g_callback(buffer, filled, g_user_data);
        }
        
        return filled;
    }
    
    return filled;
}
