#include "RevProxy.h"
#include <iostream>
#include <unistd.h>



void display_exit()
{
    std::string help_msg = "INFO: This proxy server is intended to be ran as a web proxy, by default it will treat all requests as HTTP and logged accordingly.\n"
                            "USAGE: ./rev-proxy <host port> <peer IP> <peer port> -<option> <arguments>...\n"
                            "OPTIONS: -https <certificate path> <key path>: use https, -d: daemonize.\n"
                            "NOTE: The https option only provides https on the listening side, traffic forwarded to the specified <peer IP> will be unencrypted.\n";
    std::cout << help_msg;
    exit(0);
}

std::unique_ptr<RevProxy> proxy_factory(int argc, char** argv)
{
    if(argc < 4)
        display_exit();

    bool https = false;;   
    int host_port = atoi(argv[1]);
    int peer_port = atoi(argv[3]);
    std::string peer_IP(argv[2]), cert_path, key_path;
    if(peer_IP == "localhost") 
        peer_IP = "127.0.0.1";

    std::unique_ptr<RevProxy> proxy;
    for(int i = 4; i < argc; i++)
    {
        if(!strcmp(argv[i], "-d"))
        {
            pid_t pid = fork();
            if(pid != 0)
                exit(0);
        }
        if(!strcmp(argv[i], "-https"))
        {
            if(argc < i + 3)
                display_exit();

            cert_path = argv[i+1];
            key_path = argv[i+2];
            https = true;
        }
    }

    if(https)
        return std::make_unique<RevProxy>(host_port, peer_IP, peer_port, cert_path, key_path, true);
    return std::make_unique<RevProxy>(host_port, peer_IP, peer_port); 
}

int main(int argc, char** argv)
{
    if(argc == 1 || argv[1] == "-h")
    {
        display_exit();
    }
    
    std::unique_ptr<RevProxy> proxy = proxy_factory(argc, argv);   
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