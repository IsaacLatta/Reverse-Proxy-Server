
#ifndef SESSION_H
#define SESSION_H

#include <asio.hpp>
#include <vector>
#include <memory>
#include <iostream>

#include "logger.h"

#define BUFFER_SIZE 65536

class Session : public std::enable_shared_from_this<Session>
{
    public:
    Session(asio::ip::tcp::socket sock);
    void start(std::shared_ptr<asio::ip::tcp::socket> _backend_sock);
    asio::ip::tcp::socket& get_socket();
    std::string clientIP();
    
    private:
    std::shared_ptr<asio::ip::tcp::socket> _sock;
    
    void write_backend(const asio::error_code& error, const std::shared_ptr<asio::ip::tcp::socket>& backend_sock, const std::shared_ptr<std::vector<char>>& buffer);
    void forward_response(const asio::error_code& error, const std::shared_ptr<asio::ip::tcp::socket>& backend_sock, const std::shared_ptr<std::vector<char>>& buffer, std::shared_ptr<std::size_t> bytes_read);
};

#endif
