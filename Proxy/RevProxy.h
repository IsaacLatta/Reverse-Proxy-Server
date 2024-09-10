#ifndef REVPROXY_H
#define REVPROXY_H

#include <asio.hpp>
#include <string>
#include <memory>
#include "Session.h"

class RevProxy
{
    public:
    RevProxy(int local_port, const std::string& server_ip, int server_port);
    void run();

    private:
    //asio::ssl::context ssl_context;
    asio::io_context _io_context;
    std::shared_ptr<asio::ip::tcp::acceptor> _acceptor;
    asio::ip::tcp::endpoint _endpoint;
    asio::ip::tcp::endpoint _backend_endpoint;
    std::string _backend_IP;
    int _port, _backend_port;

    //void load_certificate(const std::string& cert_path, const std::string& key_path);
    void accept_caller(std::shared_ptr<Session> session);
    void accept_handler(const asio::error_code& error, const std::shared_ptr<Session>& session);
};

#endif