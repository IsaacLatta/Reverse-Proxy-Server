#include "RevProxy.h"

RevProxy::RevProxy(int local_port, const std::string& server_IP, int server_port, const std::string& cert_path, const std::string& key_path, bool ssl) 
    : _ssl_context(asio::ssl::context::tlsv12), 
    _port(local_port), 
    _backend_port(server_port), 
    _backend_IP(server_IP), 
    _ssl(ssl)
{
    this->_endpoint = asio::ip::tcp::endpoint(asio::ip::tcp::v4(), this->_port);
    _acceptor = std::make_shared<asio::ip::tcp::acceptor>(this->_io_context, this->_endpoint);
    _backend_endpoint = asio::ip::tcp::endpoint(asio::ip::address::from_string(this->_backend_IP), this->_backend_port);

    if(_ssl && !cert_path.empty() && !key_path.empty())
    {
        load_certificate(cert_path, key_path);
    }
}

void RevProxy::load_certificate(const std::string& cert_path, const std::string& key_path)
{
    this->_ssl_context.set_options(asio::ssl::context::default_workarounds | // workaround common bugs
                                  asio::ssl::context::no_sslv2 | // disable sslv2
                                  asio::ssl::context::single_dh_use); // enable new dh use for each session
    this->_ssl_context.use_certificate_chain_file(cert_path);
    this->_ssl_context.use_private_key_file(key_path, asio::ssl::context::pem); // privacy enhanced mail format
}

void RevProxy::run()
{
    auto session = std::make_shared<Session>(socket_factory());
    this->_acceptor->async_accept(session->get_socket()->get_raw_socket(),
    [this, session](const asio::error_code& error)
    {
        this->accept_handler(error, session);
    });
    
    logger::log(nullptr, "INFO proxy running");
    this->_io_context.run();
}

void RevProxy::accept_caller(std::shared_ptr<Session> session)
{
    this->_acceptor->async_accept(session->get_socket()->get_raw_socket(),
    [this, session](const asio::error_code& error)
    {
        this->accept_handler(error, session);
    });
}

void RevProxy::accept_handler(const asio::error_code& error, const std::shared_ptr<Session>& session)
{
    if(error)
    {
        logger::debug("ERROR", "async_accept", error.message(), __FILE__, __LINE__);
        accept_caller(session);
        return;
    }
    
    auto backend_sock = std::make_unique<HTTPSocket>(this->_io_context); // replace with factory function later
    asio::error_code ec;
    backend_sock->get_raw_socket().connect(this->_backend_endpoint, ec);
    if(ec)
    {
        logger::log(nullptr, "ERROR " + ec.message());
        logger::debug("ERROR", "connect", ec.message(), __FILE__, __LINE__);
        accept_caller(std::make_shared<Session>(socket_factory()));
        return;
    }
    session->start(std::move(backend_sock)); 
    accept_caller(std::make_shared<Session>(socket_factory()));
}

std::unique_ptr<Socket> RevProxy::socket_factory()
{
    if (this->_ssl)
        return std::make_unique<HTTPSSocket>(this->_io_context, this->_ssl_context);

    return std::make_unique<HTTPSocket>(this->_io_context);
}
