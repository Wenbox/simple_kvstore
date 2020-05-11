//
// Created by wxu on 08.05.20.
//

#define ASIO_DISABLE_THREADS

#include <iostream>
#include <string>
#include <asio.hpp>
#include "kv_store.h"
#include "message_tag.h"
#include "request_session.h"
#include "server.h"


using asio::ip::tcp;

server::server(asio::io_context &io_context, int port) : m_io_context(io_context),
                                               m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
}

void server::start_accept() {
    request_session::pointer new_connection = request_session::create(m_io_context, m_store);
    m_acceptor.async_accept(new_connection->socket(), std::bind(&server::handle_accept, this,
                                                                new_connection, std::placeholders::_1));
}

void server::handle_accept(request_session::pointer new_connection, const asio::error_code &error) {
    if (!error) {
        new_connection->start();
    }
    start_accept();

}

int main() {
    try {
        asio::io_context io_context;
        server server(io_context, 1313);
        io_context.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}