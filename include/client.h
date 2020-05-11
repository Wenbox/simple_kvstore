//
// Created by wxu on 11.05.20.
//

#ifndef SIMPLE_KVSTORE_CLIENT_H
#define SIMPLE_KVSTORE_CLIENT_H
#include <asio.hpp>
#include <string>
#include <vector>
#include <memory>
#include <stdint.h>

using asio::ip::tcp;

class client {
public:
    client(const std::string& host, const std::string& port);
    std::string get(const std::string& key, message_tag& status);
    void put(const std::string& key, const std::string& value, message_tag& status);
    void erase(const std::string& key, message_tag& status);
    std::vector<std::pair<std::string, std::string> >  scan(const std::string& lower,
                                                                const std::string& upper,
                                                                message_tag& status);
private:
    std::string request_server(std::string& out_msg);


    asio::io_context m_io_context;
    tcp::resolver::results_type m_endpoint;
    std::vector<char> m_inbound;
};
#endif //SIMPLE_KVSTORE_CLIENT_H
