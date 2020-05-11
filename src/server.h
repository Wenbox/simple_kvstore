//
// Created by wxu on 10.05.20.
//

#ifndef SIMPLE_KVSTORE_SERVER_H
#define SIMPLE_KVSTORE_SERVER_H

#include <asio.hpp>
#include "kv_store.h"

class server {
public:
    server(asio::io_context &io_context, int port);

private:
    void start_accept();
    void handle_accept(request_session::pointer new_connection, const asio::error_code &error);

    kv_store m_store;
    asio::io_context &m_io_context;
    tcp::acceptor m_acceptor;
};
#endif //SIMPLE_KVSTORE_SERVER_H
