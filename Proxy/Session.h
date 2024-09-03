
#ifndef SESSION_H
#define SESSION_H

#include <asio.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>

#include "logger.h"

#define BUFFER_SIZE 65536

class Session : public std::enable_shared_from_this<Session>
{
    public:
    long bytes_transferred;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> RTT_start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;

    Session(asio::ip::tcp::socket sock);
    void start(asio::ip::tcp::socket backend_sock);
    asio::ip::tcp::socket& get_socket();
    std::string clientIP();
    
    private:
    std::shared_ptr<asio::ip::tcp::socket> _sock;
    std::shared_ptr<asio::ip::tcp::socket> _backend_sock;
    std::vector<char> buffer;

    void write_backend(const asio::error_code& error);
    void forward_response(const asio::error_code& error, std::shared_ptr<std::size_t> bytes_read);
};

#endif
