#ifndef SOCKET_H
#define SOCKET_H

#include <asio.hpp>
#include <asio/ssl.hpp>
#include <vector>
#include "logger.h"

class Socket
{
    public:
    virtual void do_handshake(const std::function<void(const asio::error_code&)>& callback) = 0;
    virtual void do_read(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) = 0;
    virtual void do_write(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) = 0;
    virtual std::string get_IP() = 0;
    virtual asio::ip::tcp::socket& get_raw_socket() = 0;
    virtual ~Socket() = default;
};

class HTTPSocket: public Socket
{
    public:
    HTTPSocket(asio::io_context& io_context) : _socket(io_context) {};
    void do_handshake(const std::function<void(const asio::error_code&)>& callback) override;
    void do_read(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) override;
    void do_write(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) override;
    std::string get_IP() override;
    asio::ip::tcp::socket& get_raw_socket() override;
    ~HTTPSocket() override;
    private:
    asio::ip::tcp::socket _socket;
};

class HTTPSSocket : public Socket
{
    public:
    HTTPSSocket(asio::io_context& io_context, asio::ssl::context& ssl_context): _socket(io_context, ssl_context) {};
    void do_handshake(const std::function<void(const asio::error_code&)>& callback) override;
    void do_read(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) override;
    void do_write(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback) override;
    std::string get_IP() override;
    asio::ip::tcp::socket& get_raw_socket() override;
    ~HTTPSSocket() override;
    private:
    asio::ssl::stream<asio::ip::tcp::socket> _socket;
};

#endif

