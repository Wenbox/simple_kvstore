//
// Created by wxu on 10.05.20.
//

#include "request_session.h"
#include "config.h"
request_session::pointer request_session::create(asio::io_context &io_context, kv_store &store) {
    return pointer(new request_session(io_context, store));
}

tcp::socket &request_session::socket() {
    return m_socket;
}

void request_session::start() {
    do_read_header();
}

//read the message tag, denote the requested operation.
void request_session::do_read_header() {

    auto self(shared_from_this());
    asio::error_code ec;
    m_socket.async_read_some(asio::buffer(m_buffer, 1),
                            [this, self](asio::error_code ec, size_t size_read) {
                                if (!ec && size_read == 1) {
                                    do_read_body(m_buffer[0]);
                                } else {
                                    reply(message_tag::UNKNOWN_OP);

                                }
                            });
}

//read the payload of the request
void request_session::do_read_body(char tag) {
    auto self(shared_from_this());
    switch (tag) {
        case message_tag::GET:
            m_socket.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                    [this, self](asio::error_code ec, std::size_t sz) {
                                        if (!ec && sz == config::KEY_SIZE) {
                                            std::string key(m_buffer, config::KEY_SIZE);
                                            std::string value = m_store.get(key);
                                            if (!value.empty())
                                                reply(message_tag::OK, value);
                                            else
                                                reply(message_tag::NOT_FOUND);
                                        } else if(!ec){
                                            reply(message_tag::ILLEGAL_PARA);
                                        } else {
                                            reply(message_tag::ERROR);
                                        }
                                    });
            break;
        case message_tag::PUT:
            m_socket.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                    [this, self](asio::error_code ec, std::size_t sz) {
                                        if (!ec && sz == config::KEY_SIZE + config::VALUE_SIZE) {
                                            std::string key(m_buffer, config::KEY_SIZE);
                                            std::string value(m_buffer + config::KEY_SIZE, config::VALUE_SIZE);
                                            try {
                                                m_store.put(key, value);
                                            } catch (...) {
                                                std::cerr << "Error, unable to log\n";
                                                reply(message_tag::ERROR);
                                            }
                                            reply(message_tag::OK);
                                        } else if(!ec){
                                            reply(message_tag::ILLEGAL_PARA);
                                        } else {
                                            reply(message_tag::ERROR);
                                        }
                                    });
            break;
        case message_tag::DELETE:
            m_socket.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                    [this, self](asio::error_code ec, std::size_t sz) {
                                        if (!ec && sz == config::KEY_SIZE) {
                                            std::string key(m_buffer, config::KEY_SIZE);
                                            try {
                                                m_store.erase(key);
                                            } catch (...) {
                                                std::cerr << "Error, unable to log\n";
                                                reply(message_tag::ERROR);
                                            }
                                            reply(message_tag::OK);
                                        } else if(!ec){
                                            reply(message_tag::ILLEGAL_PARA);
                                        } else {
                                            reply(message_tag::ERROR);
                                        }

                                    });
            break;
        case message_tag::SCAN:
            m_socket.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                    [this, self](asio::error_code ec, std::size_t sz) {
                                        if (!ec && sz == 2 * config::KEY_SIZE) {
                                            std::string lb(m_buffer, config::KEY_SIZE);
                                            std::string ub(m_buffer + config::KEY_SIZE, config::KEY_SIZE);
                                            std::string res = m_store.scan(lb, ub);
                                            reply(message_tag::OK, res);
                                        } else if(!ec){
                                            reply(message_tag::ILLEGAL_PARA);
                                        } else {
                                            reply(message_tag::ERROR);
                                        }
                                    });
            break;
        default:
            reply(message_tag::UNKNOWN_OP);

    }
}

//reply to client with payload
void request_session::reply(message_tag t, std::string &msg) {
    std::string reply(1, t);
    reply += msg;
    asio::error_code ec;
    asio::write(m_socket, asio::buffer(reply), ec);
}

//reply without payload
void request_session::reply(message_tag t) {
    m_buffer[0] = t;
    m_socket.send(asio::buffer(m_buffer, 1));
}


request_session::request_session(asio::io_context &io_context, kv_store &store) :
        m_socket(io_context),
        m_store(store) {
}
