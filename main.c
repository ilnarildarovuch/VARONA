#include "bruteforce/ssh_bruteforce.c"
#include "bruteforce/telnet_bruteforce.c"

int main() {
    ssh_brute();
    telnet_brute();
    return 0;
}