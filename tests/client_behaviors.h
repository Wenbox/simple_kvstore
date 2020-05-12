//
// Created by wxu on 12.05.20.
//

#ifndef SIMPLE_KVSTORE_CLIENT_BEHAVIORS_H
#define SIMPLE_KVSTORE_CLIENT_BEHAVIORS_H

#include <string>
#include <iostream>
#include <asio.hpp>
#include "client.h"
#include "stopwatch.h"
#include "message_tag.h"
#include "kv_store_config.h"

void benign_client(std::string &host, int id, double &time_elapsed) {
    client c(host, std::to_string(kv_store_config::PORT));
    int num_of_keys = 10000;
    stopwatch timer;
    //construct key sets for test
    std::vector<std::string> keys;
    std::vector<std::string> values;
    for (int i = 0; i < num_of_keys; ++i) {
        std::string key = std::to_string(i);
        //separate key space for every client
        key = std::string(8 - key.size(), '0') + key;
        key[0] += id;
        keys.push_back(key);
        values.push_back(std::string(256 - key.size(), '0') + key);
    }
    message_tag status;

    //make sure the keys for testing are cleared
    for (auto &key : keys) {
        c.erase(key, status);
        if (status != message_tag::OK)//something wrong
        {
            std::cerr << "Connection failure, abort!\n";
            return;
        }
    }

    timer.start();
    //GET
    std::cout << id << " GET\n";
    for (auto &key : keys) {
        auto response = c.get(key, status);
        assert(status == message_tag::NOT_FOUND && response.empty());
    }

    //PUT
    std::cout << id << " PUT\n";
    for (int i = 0; i < num_of_keys; ++i) {
        c.put(keys[i], values[i], status);
        assert(status == message_tag::OK);
    }

    //GET
    std::cout << id << " GET\n";
    for (auto &key : keys) {
        auto response = c.get(key, status);
        assert(status == message_tag::OK);
    }


    //SCAN
    std::cout << id << " SCAN\n";
    {
        auto pairs = c.scan(keys.front(), keys.back(), status);
        assert(status == message_tag::OK && pairs.size() == keys.size());

        pairs = c.scan(*(keys.begin()), *(keys.begin() + keys.size() / 2), status);
        assert(status == message_tag::OK && pairs.size() == keys.size() / 2 + 1);

        pairs = c.scan(keys.back(), keys.front(), status);
        assert(status == message_tag::OK && pairs.empty());
    }


    //DELETE
    std::cout << id << " DELETE\n";
    for (size_t i = 0; i < keys.size(); i += 2) {
        c.erase(keys[i], status);
        assert(status == message_tag::OK);
    }

    //GET after DELETE
    std::cout << id << " GET\n";
    for (size_t i = 0; i < keys.size(); ++i) {
        auto response = c.get(keys[i], status);
        if (i % 2 == 0)
            assert(status == message_tag::NOT_FOUND && response.empty());
        else
            assert(status == message_tag::OK && response.size() == 256);
    }

    //SCAN after DELETE
    std::cout << id << " SCAN\n";
    {
        auto pairs = c.scan(keys.front(), keys.back(), status);
        assert(status == message_tag::OK && pairs.size() == keys.size() / 2);
    }
    timer.stop();
    time_elapsed = timer.elapsed();


}

void connect_and_block(std::string &host, int id) {
    using asio::ip::tcp;

    std::cout << id << " holds connection\n";
    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(kv_store_config::PORT));
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
}

void send_faulty(std::string &host, int id) {
    using asio::ip::tcp;

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints = resolver.resolve(host, std::to_string(kv_store_config::PORT));
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    //send wrong tag
    std::cout << id << " sends wrong OP\n";
    std::vector<char> dummy(128);
    asio::error_code ec;
    for (int i = 0; i < 1000; ++i) {
        dummy[0] = message_tag::OK;
        asio::write(socket, asio::buffer(dummy), ec);
        size_t l = asio::read(socket, asio::buffer(dummy), ec);
        assert(ec == asio::error::eof || (l == 1 && dummy[0] == message_tag::UNKNOWN_OP));
    }

    //wrong size of key or values
    std::cout << id << " sends wrong size\n";
    for (int i = 0; i < 1000; ++i) {
        dummy[0] = message_tag::GET;
        asio::write(socket, asio::buffer(dummy), ec);
        size_t l = asio::read(socket, asio::buffer(dummy), ec);
        assert(ec == asio::error::eof || (l == 1 && dummy[0] == message_tag::ILLEGAL_PARA));
    }


}

#endif //SIMPLE_KVSTORE_CLIENT_BEHAVIORS_H
