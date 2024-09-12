
#include "Session.h"

Socket* Session::get_socket()
{
    return this->_sock.get();
}


void Session::forward_response(const asio::error_code& error, std::size_t bytes_read)
{
    auto self = shared_from_this();

    self->_backend_sock->do_read(self->_buffer.data() + bytes_read, self->_buffer.size() - bytes_read,
    [self, bytes_read](const asio::error_code& error, std::size_t bytes) mutable
    {
        if(error && error != asio::error::eof)
        {
            logger::debug("ERROR", "async_read_some", error.message() , __FILE__, __LINE__);
            logger::log(self, "ERROR " + error.message());
            return;
        }
        if(error == asio::error::eof)
        {
            self->end_time = std::chrono::system_clock::now();
            logger::log(self, "INFO");
            return;
        }
        self->bytes_transferred += bytes;
        bytes_read += bytes;
        self->_sock->do_write(self->_buffer.data() + bytes_read - bytes, bytes,
        [self, bytes_read] (const asio::error_code& error, std::size_t bytes_written) mutable 
        {
            if(self->_buffer.size() - bytes_read < 10000)
            {
                memset(self->_buffer.data(), '\0', self->_buffer.size());
                bytes_read = 0;
            }
            self->forward_response(error, bytes_read);
        });
    });
}

void Session::write_backend(const asio::error_code& error)
{
    auto self = shared_from_this();

    if(error && error != asio::error::eof)
    {
        logger::debug("ERROR", "async_read_some", error.message() , __FILE__, __LINE__);
        logger::log(self, "ERROR" + error.message());
        return;
    }

    logger::debug("REQUEST HEADER", "", std::string(self->_buffer.begin(), self->_buffer.end()), __FILE__, __LINE__);
    self->req_header = logger::get_header(self->_buffer);
    self->user_agent = logger::get_user_agent(self->_buffer);
    self->_backend_sock->do_write(self->_buffer.data(), self->_buffer.size(),
    [self](const asio::error_code& error, std::size_t bytes)
    {
        if(error)
        {
            logger::debug("ERROR", "async_write", error.message() , __FILE__, __LINE__);
            logger::log(self, "ERROR" + error.message());
            return;
        }
        self->RTT_start_time = std::chrono::system_clock::now();
        std::size_t bytes_read = 0;
        memset(self->_buffer.data(), '\0', self->_buffer.size());
        self->forward_response(error, bytes_read);
    });
}

void Session::start(std::unique_ptr<Socket>&& backend_sock)
{
    auto self = shared_from_this();
    
    self->_backend_sock = std::move(backend_sock);

    self->_buffer.resize(BUFFER_SIZE);
    self->start_time = std::chrono::system_clock::now();
    self->_sock->do_handshake(
    [self](const asio::error_code& error)
    {
        if(error)
        {
            logger::debug("ERROR", "do_handshake", error.message(), __FILE__, __LINE__);
            return;
        }
        self->_sock->do_read(self->_buffer.data(), self->_buffer.size(), 
        [self](const asio::error_code& error, std::size_t bytes)
        {
            self->bytes_transferred += bytes;
            self->write_backend(error);
        });
    });
}

