#include "RevProxy.h"
#include <iostream>


void display_exit()
{
    std::string help_msg = "USAGE: ./rev-proxy <host_port> <peer_IP> <peer_port> -<option> <arguments>...\n"
                            "OPTIONS: -d, daemon the proxy, -https, use https\n";
    std::cout << help_msg;
    exit(0);
}

int main(int argc, char** argv)
{
    if( argc == 1 || argv[1] == "-h")
    {
        display_exit();
    }

    std::unique_ptr<RevProxy> proxy;
    int host_port = atoi(argv[1]);
    int peer_port = atoi(argv[3]);
    std::string peer_IP(argv[2]), cert_path, key_path;
    if(peer_IP == "localhost") peer_IP = "127.0.0.1";

    if(argc >= 6 && !strcmp(argv[4],"-https"))
    {
        cert_path = argv[5];
        std::cout << "Cert path: " << cert_path << "\n";
        key_path = argv[6];
        std::cout << "Key path: " << key_path << "\n";
        proxy = std::make_unique<RevProxy>(host_port, peer_IP, peer_port, cert_path, key_path, true);
    }
    else
         proxy = std::make_unique<RevProxy>(host_port, peer_IP, peer_port);        
    try
    {
        proxy->run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    return 0;
}