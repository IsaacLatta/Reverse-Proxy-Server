#include "Socket.h"

asio::ip::tcp::socket& HTTPSocket::get_raw_socket()
{
    return this->_socket;
}

void HTTPSocket::do_handshake(const std::function<void(const asio::error_code&)>& callback)
{
    asio::error_code ec;
    callback(ec);
}

void HTTPSocket::do_write(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback)
{
    asio::async_write(this->_socket, asio::buffer(buffer, buffer_size), 
    [this, callback](const std::error_code& error, std::size_t bytes)
    {
        callback(error, bytes);
    });
}

void HTTPSocket::do_read(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback)
{
    this->_socket.async_read_some(asio::buffer(buffer, buffer_size),    
    [this, callback](const asio::error_code& error, std::size_t bytes)
    {
        callback(error, bytes);
    });
}

asio::ip::tcp::socket& HTTPSSocket::get_raw_socket()
{
    return this->_socket.next_layer();
}

void HTTPSSocket::do_handshake(const std::function<void(const std::error_code& error)>& callback)
{
    this->_socket.async_handshake(asio::ssl::stream_base::server,
    [this, callback](const asio::error_code& error)
    {
        callback(error);
    });
}

void HTTPSSocket::do_read(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback)
{
    this->_socket.async_read_some(asio::buffer(buffer, buffer_size),    
    [this, callback](const asio::error_code& error, std::size_t bytes)
    {
        callback(error, bytes);
    });
}

void HTTPSSocket::do_write(char* buffer, std::size_t buffer_size, const std::function<void(const asio::error_code&, std::size_t)>& callback)
{
    asio::async_write(this->_socket, asio::buffer(buffer, buffer_size), 
    [this, callback](const std::error_code& error, std::size_t bytes)
    {
        callback(error, bytes);
    });
}