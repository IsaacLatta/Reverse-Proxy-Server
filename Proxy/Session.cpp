
#include "Session.h"

Session::Session(asio::ip::tcp::socket sock)
{
    this->_sock = std::make_shared<asio::ip::tcp::socket>(std::move(sock));
}

std::string Session::clientIP()
{
    return this->_sock->remote_endpoint().address().to_string();
}

asio::ip::tcp::socket& Session::get_socket()
{
    return *(this->_sock);
}

void Session::forward_response(const asio::error_code& error, const std::shared_ptr<asio::ip::tcp::socket>& backend_sock, const std::shared_ptr<std::vector<char>>& buffer, std::shared_ptr<std::size_t> bytes_read)
{
    auto self = shared_from_this();

    backend_sock->async_read_some(asio::buffer(buffer->data() + *bytes_read, buffer->size() - *bytes_read), 
    [self, backend_sock, buffer, bytes_read](const asio::error_code& error, std::size_t bytes)
    {
        if(error && error != asio::error::eof)
        {
            logger::log(self->clientIP(), "ERROR", "async_read_some", error.message() , __FILE__, __LINE__);
            self->_sock->close();
            backend_sock->close();
            return;
        }

        if(error == asio::error::eof)
        {
            logger::log(self->clientIP(), "INFO", "session ended", "", __FILE__, __LINE__);
            self->_sock->close();
            backend_sock->close();
            return;
        }

        *bytes_read += bytes;
        asio::async_write(*(self->_sock), asio::buffer(buffer->data() + *bytes_read - bytes, bytes),
        [self, backend_sock, buffer, bytes_read](const asio::error_code& error, std::size_t bytes)
        {
            if(BUFFER_SIZE - *bytes_read < 10000)
            {
                *bytes_read = 0;
                memset(buffer->data(), 0, buffer->size());
            }
            self->forward_response(error, backend_sock, buffer, bytes_read);
        }); 
    });
}

void Session::write_backend(const asio::error_code& error, const std::shared_ptr<asio::ip::tcp::socket>& backend_sock, const std::shared_ptr<std::vector<char>>& buffer)
{
    auto self = shared_from_this();

    if(error)
    {
        logger::debug("ERROR", "async_read_until", error.message() , __FILE__, __LINE__);
        self->_sock->close();
        backend_sock->close();
        return;
    }

    logger::debug("REQUEST HEADER", "", std::string(buffer->begin(), buffer->end()), __FILE__, __LINE__);

    asio::async_write(*backend_sock, asio::buffer(*buffer),
    [self, backend_sock, buffer](const asio::error_code& error, std::size_t bytes)
    {
        if(error)
        {
            logger::log(self->clientIP(), "ERROR", "async_write", error.message() , __FILE__, __LINE__);
            self->_sock->close();
            backend_sock->close();
            return;
        }
        logger::log(self->clientIP(), "INFO", "bytes written to backend", std::to_string(bytes), __FILE__, __LINE__);
        auto bytes_read = std::make_shared<std::size_t>(0);
        auto resp_buffer = std::make_shared<std::vector<char>> (BUFFER_SIZE);
        self->forward_response(error, backend_sock, resp_buffer, bytes_read);
    });
}

void Session::start(std::shared_ptr<asio::ip::tcp::socket> backend_sock)
{
    auto self = shared_from_this();
    
    logger::log(self->clientIP(), "INFO", "session started", "", __FILE__, __LINE__);
    auto buffer = std::make_shared<std::vector<char>> (BUFFER_SIZE);
    self->_sock->async_read_some(asio::buffer(*buffer), // read the http header
    [self, backend_sock, buffer](const asio::error_code& error, std::size_t bytes)
    {
        logger::log(self->clientIP(), "INFO", "bytes received", std::to_string(bytes), __FILE__, __LINE__);
        self->write_backend(error, backend_sock, buffer);
    });
}

