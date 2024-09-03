#include "RevProxy.h"

RevProxy::RevProxy(int local_port, const std::string& server_IP, int server_port)
{
    this->_port = local_port;
    this->_backend_port = server_port;
    this->_backend_IP = server_IP;
    this->_endpoint = std::move(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), this->_port));
    this->_acceptor = std::make_shared<asio::ip::tcp::acceptor>(asio::ip::tcp::acceptor(this->_io_context, this->_endpoint));
    this->_backend_endpoint =  std::move(asio::ip::tcp::endpoint(asio::ip::address::from_string(this->_backend_IP), this->_backend_port));
}

void RevProxy::run()
{
    auto session = std::make_shared<Session>(asio::ip::tcp::socket(this->_io_context));
    this->_acceptor->async_accept(session->get_socket(),
    [this, session](const asio::error_code& error)
    {
        this->accept_handler(error, session);
    });
    
    logger::log(nullptr, "INFO proxy running");
    this->_io_context.run();
}

void RevProxy::accept_handler(const asio::error_code& error, const std::shared_ptr<Session>& session)
{
    if(error)
    {
        logger::debug("ERROR", "async_accept", error.message() , __FILE__, __LINE__);
        this->_acceptor->async_accept(session->get_socket(),
        [this, session](const asio::error_code& error)
        {
            this->accept_handler(error, session);
        });
        return;
    }
    std::string client_IP = session->clientIP();
    
    asio::ip::tcp::socket backend_sock(this->_io_context);
    asio::error_code ec;
    backend_sock.connect(this->_backend_endpoint, ec);
    if(ec)
    {
        logger::log(nullptr, "ERROR " + ec.message());
        logger::debug("ERROR", "connect", ec.message(), __FILE__, __LINE__);
        return;
    }
    
    logger::debug("INFO", "connected to server, client", backend_sock.remote_endpoint().address().to_string() , __FILE__, __LINE__);
    session->start(std::move(backend_sock)); 

    auto new_session = std::make_shared<Session>(asio::ip::tcp::socket(this->_io_context));
    this->_acceptor->async_accept(new_session->get_socket(),
    [this, new_session](const asio::error_code& error)
    {
        this->accept_handler(error, new_session);
    });
}
