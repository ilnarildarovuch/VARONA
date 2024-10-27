#include "telibs/telnet_lib.h"
#include "telnet_check.c"

int main(int argc, char **argv) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    telnet_chk(sock, argc, argv);
    
    //TELNET
    int pid;    
    if ((pid = fork())) {
        send_cmd(sock, pid);
    } else {
        receive(sock);
    }
    //TELNET
    
    return 0;
}
