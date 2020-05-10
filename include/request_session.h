//
// Created by wxu on 10.05.20.
//

#ifndef SIMPLE_KVSTORE_REQUEST_SESSION_H
#define SIMPLE_KVSTORE_REQUEST_SESSION_H

#include <asio.hpp>
#include <kv_store.h>
#include <iostream>
#include <message_tag.h>

using asio::ip::tcp;
class request_session: public std::enable_shared_from_this<request_session> {
public:
    typedef std::shared_ptr<request_session> pointer;

    static pointer create(asio::io_context &io_context, kv_store &store);
    tcp::socket &socket();
    void start();

private:
    void do_read_header();

    void do_read_body(char tag);

    void reply(message_tag t, std::string &msg);

    void reply(message_tag t);

    request_session(asio::io_context &io_context, kv_store &store);

    tcp::socket m_socket;
    enum {
        MAX_BUFFER_SIZE = 8192
    };
    char m_buffer[MAX_BUFFER_SIZE];
    kv_store &m_store;
};


#endif //SIMPLE_KVSTORE_REQUEST_SESSION_H
