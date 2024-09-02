#include "RevProxy.h"
#include <iostream>

// ./rev-proxy <host_port> <peer_IP> <peer_port>
int main(int argc, char** argv)
{
    if(argc != 4 || argv[1] == "-h")
    {
        std::cout << "USAGE: ./rev-proxy <host_port> <peer_IP> <peer_port>\n";
        return 0;
    }

    try
    {
        int host_port = atoi(argv[1]);
        int peer_port = atoi(argv[3]);
        std::string peer_IP(argv[2]);
        RevProxy proxy(host_port, peer_IP, peer_port);
        proxy.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}