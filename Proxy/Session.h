
#ifndef SESSION_H
#define SESSION_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <vector>
#include <memory>
#include <iostream>
#include <chrono>

#include "Socket.h"
#include "logger.h"

#define BUFFER_SIZE 65536

class Session : public std::enable_shared_from_this<Session>
{
    public:
    long bytes_transferred;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> RTT_start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;

    Session(std::unique_ptr<Socket>&& sock) : _sock(std::move(sock)), bytes_transferred(0) {};
    void start(std::unique_ptr<Socket>&& backend_sock);
    Socket* get_socket();
    
    private:
    std::unique_ptr<Socket> _sock;
    std::unique_ptr<Socket> _backend_sock;
    std::vector<char> buffer;
    void write_backend(const asio::error_code& error);
    void forward_response(const asio::error_code& error, std::size_t bytes_read);
};

#endif
