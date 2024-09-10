#include "RevProxy.h"

RevProxy::RevProxy(int local_port, const std::string& server_IP, int server_port)
{
    this->_port = local_port;
    this->_backend_port = server_port;
    this->_backend_IP = server_IP;
    this->_endpoint = std::move(asio::ip::tcp::endpoint(asio::ip::tcp::v4(), this->_port));
    this->_acceptor = std::make_shared<asio::ip::tcp::acceptor>(asio::ip::tcp::acceptor(this->_io_context, this->_endpoint));
    this->_backend_endpoint =  std::move(asio::ip::tcp::endpoint(asio::ip::address::from_string(this->_backend_IP), this->_backend_port));
    //this->ssl_context = asio::ssl::context(asio::ssl::context::tlsv12);
    //load_certificate(cert_path, key_path);
}

/*
void RevProxy::load_certificate(const std::string& cert_path, const std::string& key_path)
{
    this->ssl_context.set_options(asio::ssl::context::default_workarounds | // workaround common bugs
                                  asio::ssl::context::no_sslv2 | // disable sslv2
                                  asio::ssl::context::single_dh_use); // enable new dh use for each session
    this->ssl_context.use_certificate_chain_file(cert_path);
    this->ssl_context.use_private_key_file(key_path, asio::ssl::context::pem); // privacy enhanced mail format
}
*/

void RevProxy::run()
{
    auto session = std::make_shared<Session>(std::make_unique<HTTPSocket>(_io_context));
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
    logger::debug("INFO", "proxy", "waiting for client", __FILE__, __LINE__);
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
    logger::debug("INFO", "proxy", "client connected", __FILE__, __LINE__);
    auto backend_sock = std::make_unique<HTTPSocket>(this->_io_context);
    asio::error_code ec;
    backend_sock->get_raw_socket().connect(this->_backend_endpoint, ec);
    if(ec)
    {
        logger::log(nullptr, "ERROR " + ec.message());
        logger::debug("ERROR", "connect", ec.message(), __FILE__, __LINE__);
        accept_caller(std::make_shared<Session>(std::make_unique<HTTPSocket>(_io_context)));
        return;
    }
    session->start(std::move(backend_sock)); 
    logger::debug("INFO", "proxy", "session started for client", __FILE__, __LINE__);
    accept_caller(std::make_shared<Session>(std::make_unique<HTTPSocket>(_io_context)));
}
