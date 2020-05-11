//
// Created by wxu on 11.05.20.
//
#include <iostream>
#include "client.h"
void run_client() {
    client c("localhost", "1313" );
    std::vector<std::string> keys;
    for(int i = 0; i < 26; ++i) {
        keys.push_back(std::string(8, 'a' + i));
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
        std::string host;
        if (argc == 1) {
            std::cout << "Connect to localhost\n";
            host = "localhost";
        } else {
            host = std::string(argv[1]);
        }
        client c(host, "1313");
        run_client();

    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}