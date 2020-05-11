//
// Created by wxu on 08.05.20.
//

//#define ASIO_STANDALONE
//#define ASIO_DISABLE_THREADS

#include <iostream>
#include <asio/detail/array.hpp>
#include <asio.hpp>
#include <thread>

#include "kv_store_config.h"
#include "message_tag.h"
#include "client.h"

client::client(const std::string &host, const std::string &port) :
        m_io_context(), m_inbound(8192){
    try {
        tcp::resolver resolver = tcp::resolver(m_io_context);
        m_endpoint = resolver.resolve(host, port);
    } catch (const asio::system_error &ex) {
        throw;
    } catch (const std::exception &e) {
        throw;
    }
}

std::string client::get(const std::string &key, message_tag &status) {
    if (key.size() != kv_store_config::KEY_SIZE) {
        status = message_tag::ILLEGAL_PARA;
        return std::string();
    } else {
        std::string request = {message_tag::GET};
        request.insert(request.end(), key.begin(), key.end());
        std::string response = request_server(request);
        if(response.empty()) {
            //failed
            status = message_tag::ERROR;
            return std::string();
        } else {
            status = static_cast<message_tag >(response[0]);
            response.erase(response.begin());
            return response;
        }
    }
}

void client::put(const std::string &key,
                 const std::string &value,
                 message_tag &status) {
    if(key.size() != kv_store_config::KEY_SIZE && value.size() != kv_store_config::VALUE_SIZE) {
        status = message_tag::ILLEGAL_PARA;
        return;
    } else {
        std::string request = {message_tag::PUT};
        request.insert(request.end(), key.begin(), key.end());
        request.insert(request.end(), value.begin(), value.end());
        std::string response = request_server(request);
        if(response.empty()) {
            //failed
            status = message_tag::ERROR;
            return;
        } else {
            status = static_cast<message_tag >(response[0]);
            return;
        }

    }
}

void client::erase(const std::string &key, message_tag &status) {
    if(key.size() != kv_store_config::KEY_SIZE) {
        status = message_tag::ILLEGAL_PARA;
        return;
    } else {
        std::string request = {message_tag::DELETE};
        request.insert(request.end(), key.begin(), key.end());
        std::string response = request_server(request);
        if(response.empty()) {
            //failed
            status = message_tag::ERROR;
            return;
        } else {
            status= static_cast<message_tag >(response[0]);
            return;
        }
    }
}

std::vector<std::pair<std::string, std::string> >  client::scan(const std::string& lower,
                                                            const std::string& upper,
                                                            message_tag& status){
    if(lower.size() != kv_store_config::KEY_SIZE && upper.size() != kv_store_config::KEY_SIZE) {
        status = message_tag::ILLEGAL_PARA;
        return std::vector<std::pair<std::string, std::string>>();
    } else {
        std::string request;
        request.reserve(kv_store_config::KEY_SIZE * 3);
        request.push_back(message_tag::SCAN);
        request.insert(request.end(), lower.begin(), lower.end());
        request.insert(request.end(), upper.begin(), upper.end());
        std::string response = request_server(request);
        if(response.empty()) {
            status = message_tag::ERROR;
            return std::vector<std::pair<std::string, std::string>>();
        } else {
            status= static_cast<message_tag >(response[0]);
            auto itr = response.begin() + 1;
            std::vector<std::pair<std::string, std::string>> to_return;
            while(itr != response.end()) {
                to_return.push_back({std::string(itr, itr+kv_store_config::KEY_SIZE),
                                    std::string(itr+kv_store_config::KEY_SIZE, itr+kv_store_config::KEY_SIZE+kv_store_config::VALUE_SIZE)});
                itr += kv_store_config::KEY_SIZE+kv_store_config::VALUE_SIZE;
            }
            return to_return;
        }

    }
};

std::string client::request_server(std::string &out_msg) {
    try {
        tcp::socket socket(m_io_context);
        asio::error_code ec;
        asio::connect(socket, m_endpoint);
        asio::write(socket, asio::buffer(out_msg), ec);
        std::string response;
        for (;;) {
            size_t l = socket.read_some(asio::buffer(m_inbound), ec);
            response.insert(response.end(), m_inbound.begin(), m_inbound.begin() + l);
            if (ec == asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (ec) {
                throw -1;
            }

        }
        return response;

    } catch (const std::exception &ex) {
        return std::string();
    }
}

void run_client(int id) {
    client c("localhost", "1313" );
    std::vector<std::string> keys;
    for(int i = 0; i < 26; ++i) {
        keys.push_back(std::string(8, 'a' + i));
        keys.back()[0] = '0' + id;
    }
    message_tag status;
    //make sure the keys for testing are cleared
    for(auto& key : keys) {
        c.erase(key, status);
        if(status != message_tag::OK)//something wrong
        {
            std::cerr << "Connection failure, abort!\n";
            return;
        }
    }

    //test get non-existed keys
    auto response = c.get(keys[0], status);
    assert(status == message_tag::NOT_FOUND && response.empty());
    std::cout << "passed get0\n";
    //test put
    for(auto& key : keys) {
        c.put(key, std::string(256, key.back()), status);
        assert(status == message_tag::OK);
    }
    std::cout << "passed put\n";

    //test get
    for(auto& key : keys) {
        auto response = c.get(key, status);
        assert(status == message_tag::OK && response == std::string(256, key.back()));
    }
    std::cout << "passed get\n";


    //test scan
    {
        auto pairs = c.scan(keys.front(), keys.back(), status);
        assert(status == message_tag::OK && pairs.size() == keys.size());
        pairs = c.scan(*(keys.begin()+5), *(keys.begin() + 10), status);
        assert(status == message_tag::OK && pairs.size() == 6);
        for(size_t i = 0; i < pairs.size(); ++i) {
            auto& kv = pairs[i];
            assert(kv.first == *(keys.begin() + 5 + i) && kv.second.back() == kv.first.back());
        }
    }
    std::cout << "passed scan\n";

    //test delete
    for(size_t i = 0; i < keys.size(); i += 2) {
        c.erase(keys[i], status);
        assert(status == message_tag::OK);
    }

    for(size_t i = 0; i < keys.size(); ++i) {
        auto response = c.get(keys[i], status);
        if(i % 2 == 0)
            assert(status == message_tag::NOT_FOUND && response.empty());
        else
            assert(status == message_tag::OK && response.size() == 256);
    }
    std::cout << "passed delete\n";

    //test scan
    {
        auto pairs = c.scan(keys.front(), keys.back(), status);
        assert(status == message_tag::OK && pairs.size() == keys.size() / 2);
    }

    std::cout << "Passed\n";


}
int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }
        std::string host(argv[1]);
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.push_back(std::thread(run_client, i));
        }

        for (auto &th : threads)
            th.join();

    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
