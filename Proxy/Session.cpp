
#include "Session.h"

Session::Session(asio::ip::tcp::socket sock)
{
    this->_sock = std::make_shared<asio::ip::tcp::socket>(std::move(sock));
    this->bytes_transferred = 0;
}

std::string Session::clientIP()
{
    return this->_sock->remote_endpoint().address().to_string();
}

asio::ip::tcp::socket& Session::get_socket()
{
    return *(this->_sock);
}

void Session::forward_response(const asio::error_code& error, std::shared_ptr<std::size_t> bytes_read)
{
    auto self = shared_from_this();

    self->_backend_sock->async_read_some(asio::buffer(self->buffer.data() + *bytes_read, self->buffer.size() - *bytes_read), 
    [self, bytes_read](const asio::error_code& error, std::size_t bytes)
    {
        if(error && error != asio::error::eof)
        {
            logger::debug("ERROR", "async_read_some", error.message() , __FILE__, __LINE__);
            logger::log(self, "ERROR " + error.message());
            self->_sock->close();
            self->_backend_sock->close();
            return;
        }

        if(error == asio::error::eof)
        {
            self->end_time = std::chrono::system_clock::now();
            logger::log(self, "INFO");
            self->_sock->close();
            self->_backend_sock->close();
            return;
        }

        self->bytes_transferred += bytes;
        *bytes_read += bytes;
        asio::async_write(*(self->_sock), asio::buffer(self->buffer.data() + *bytes_read - bytes, bytes),
        [self, bytes_read](const asio::error_code& error, std::size_t bytes)
        {
            if(BUFFER_SIZE - *bytes_read < 10000)
            {
                *bytes_read = 0;
                memset(self->buffer.data(), 0, self->buffer.size());
            }
            self->forward_response(error, bytes_read);
        }); 
    });
}

void Session::write_backend(const asio::error_code& error)
{
    auto self = shared_from_this();

    if(error)
    {
        logger::debug("ERROR", "async_read_until", error.message() , __FILE__, __LINE__);
        logger::log(self, "ERROR" + error.message());
        self->_sock->close();
        self->_backend_sock->close();
        return;
    }

    logger::debug("REQUEST HEADER", "", std::string(self->buffer.begin(), self->buffer.end()), __FILE__, __LINE__);

    asio::async_write(*(self->_backend_sock), asio::buffer(buffer),
    [self](const asio::error_code& error, std::size_t bytes)
    {
        if(error)
        {
            logger::debug("ERROR", "async_write", error.message() , __FILE__, __LINE__);
            logger::log(self, "ERROR" + error.message());
            self->_sock->close();
            self->_backend_sock->close();
            return;
        }
        self->RTT_start_time = std::chrono::system_clock::now();
        auto bytes_read = std::make_shared<std::size_t>(0);
        memset(self->buffer.data(), '\0', self->buffer.size());
        self->forward_response(error, bytes_read);
    });
}

void Session::start(asio::ip::tcp::socket backend_sock)
{
    auto self = shared_from_this();
    
    self->_backend_sock = std::make_shared<asio::ip::tcp::socket> (std::move(backend_sock));

    self->buffer.resize(BUFFER_SIZE);
    self->start_time = std::chrono::system_clock::now();
    self->_sock->async_read_some(asio::buffer(self->buffer), 
    [self](const asio::error_code& error, std::size_t bytes)
    {
        self->bytes_transferred += bytes;
        self->write_backend(error);
    });
}

