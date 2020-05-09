//
// Created by wxu on 08.05.20.
//

#define ASIO_DISABLE_THREADS

#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>
#include <asio/placeholders.hpp>
#include "kv_store.h"
#include "message.h"

using asio::ip::tcp;


class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
    typedef std::shared_ptr<tcp_connection> pointer;

    static pointer create(asio::io_context &io_context, kv_store &store) {
        return pointer(new tcp_connection(io_context, store));
    }

    tcp::socket &socket() {
        return socket_;
    }

    void start() {
        do_read_header();

    }

private:
    void do_read_header() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(m_buffer, 1),
                                [this, self](asio::error_code ec, size_t size_read) {
                                    if (!ec && size_read == 1) {
                                        do_read_body(m_buffer[0]);
                                    } else if (ec == asio::error::eof) {
                                        std::cout << "Ended!\n";
                                    }
                                });
    }

    void do_read_body(char tag) {
        auto self(shared_from_this());
        switch (tag) {
            case message::GET:
                socket_.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                        [this, self](asio::error_code ec, std::size_t sz) {
                                            if (!ec && sz == kv_store::KEY_SIZE) {
                                                std::string key(m_buffer, kv_store::KEY_SIZE);
                                                std::string value = m_store.get(key);
                                                if(!value.empty())
                                                    reply(message::GET, value);
                                                else
                                                    reply(message::NOT_FOUND);
                                            } else {
                                                reply(message::ERROR);
                                            }
                                        });
                break;
            case message::PUT:
                socket_.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                        [this, self](asio::error_code ec, std::size_t sz) {
                                            if (!ec && sz == kv_store::KEY_SIZE + kv_store::VALUE_SIZE) {
                                                std::string key(m_buffer, kv_store::KEY_SIZE);
                                                std::string value(m_buffer + kv_store::KEY_SIZE, kv_store::VALUE_SIZE);
                                                m_store.put(key, value);
                                                reply(message::PUT);
                                            } else {
                                                reply(message::ERROR);
                                            }
                                        });
                break;
            case message::DELETE:
                socket_.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                        [this, self](asio::error_code ec, std::size_t sz) {
                                            if (!ec && sz == kv_store::KEY_SIZE) {
                                                std::string key(m_buffer, kv_store::KEY_SIZE);
                                                m_store.erase(key);
                                                reply(message::DELETE);
                                            } else {
                                                reply(message::ERROR);
                                            }
                                        });
                break;
            case message::SCAN:
                socket_.async_read_some(asio::buffer(m_buffer, MAX_BUFFER_SIZE),
                                        [this, self](asio::error_code ec, std::size_t sz) {
                                            if (!ec && sz == 2 * kv_store::KEY_SIZE) {
                                                std::string lb(m_buffer, kv_store::KEY_SIZE);
                                                std::string ub(m_buffer + kv_store::KEY_SIZE, kv_store::KEY_SIZE);
                                                std::string res = m_store.scan(lb, ub);
                                                reply(message::SCAN, res);
                                            } else {
                                                reply(message::ERROR);
                                            }
                                        });
                break;
            default:
                reply(message::UNKNOWN_OP);

        }
    }

    void reply(message::tag t, std::string &msg) {
        std::string reply(1, t);
        reply += msg;
        socket_.send(asio::buffer(reply));
    }

    void reply(message::tag t) {
        m_buffer[0] = t;
        socket_.send(asio::buffer(m_buffer, 1));
    }


    tcp_connection(asio::io_context &io_context, kv_store &store) :
            socket_(io_context),
            m_store(store) {
    }

    tcp::socket socket_;
    enum {
        MAX_BUFFER_SIZE = 8192
    };
    char m_buffer[MAX_BUFFER_SIZE];
    kv_store &m_store;
};
class key_value_server {
public:
    key_value_server(asio::io_context &io_context) : io_context_(io_context),
                                                     acceptor_(io_context, tcp::endpoint(tcp::v4(), 1313)) {
        start_accept();
    }

private:
    void start_accept() {
        tcp_connection::pointer new_connection = tcp_connection::create(io_context_, m_store);
        acceptor_.async_accept(new_connection->socket(), std::bind(&key_value_server::handle_accept, this,
                                                                   new_connection, std::placeholders::_1));
    }

    void handle_accept(tcp_connection::pointer new_connection, const asio::error_code &error) {
        if (!error) {
            new_connection->start();
        }
        start_accept();

    }

    kv_store m_store;
    asio::io_context &io_context_;
    tcp::acceptor acceptor_;
};

int main() {
    try {
        asio::io_context io_context;
        key_value_server server(io_context);
        io_context.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}