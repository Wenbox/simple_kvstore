//
// Created by wxu on 11.05.20.
//

#include <iostream>
#include <thread>
#include <kv_store_config.h>
#include "client.h"
#include "stopwatch.h"

void run_testcase_1(std::string& host) {
    client c(host, "1313");
    int num_of_keys = 10000;
    stopwatch timer;
    //construct key sets for test
    std::vector<std::string> keys;
    std::vector<std::string> values;
    for(int i = 0; i < num_of_keys; ++i) {
        std::string key = std::to_string(i);
        key = std::string(8 - key.size(), '0') + key;
        keys.push_back(key);
        values.push_back(std::string(256 - key.size(), '0') + key);
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
    timer.start();
    for(auto& key : keys) {
        auto response = c.get(key, status);
        assert(status == message_tag::NOT_FOUND && response.empty());
    }
    timer.stop();
    std::cout << "get " << num_of_keys <<" non-existing keys in " << timer.elapsed() << " seconds\n" ;

    //test put
    timer.reset();
    timer.start();
    for(int i = 0; i < num_of_keys; ++i) {
        c.put(keys[i], values[i], status);
        assert(status == message_tag::OK);
    }
    timer.stop();
    std::cout << "put " << num_of_keys <<" keys in " << timer.elapsed() << " seconds\n" ;

    //test get
    timer.reset();
    timer.start();
    for(auto& key : keys) {
        auto response = c.get(key, status);
        assert(status == message_tag::OK);
    }
    timer.stop();
    std::cout << "get " << num_of_keys <<" keys in " << timer.elapsed() << " seconds\n" ;


    //test scan
    {
        timer.reset();
        timer.start();
        auto pairs = c.scan(keys.front(), keys.back(), status);
        timer.stop();

        assert(status == message_tag::OK && pairs.size() == keys.size());
        std::cout << "scan " << pairs.size() <<" keys in " << timer.elapsed() << " seconds\n" ;

        timer.reset();
        timer.start();
        pairs = c.scan(*(keys.begin()), *(keys.begin() + 5000), status);
        timer.stop();
        assert(status == message_tag::OK && pairs.size() == 5001);
        for(size_t i = 0; i < pairs.size(); ++i) {
            auto& kv = pairs[i];
            assert(kv.first == *(keys.begin() + i) );
        }
        timer.stop();
        std::cout << "scan " << pairs.size() <<" keys in " << timer.elapsed() << " seconds\n" ;
    }


    //test delete
    timer.reset();
    timer.start();
    for(size_t i = 0; i < keys.size(); i += 2) {
        c.erase(keys[i], status);
        assert(status == message_tag::OK);
    }
    timer.stop();
    std::cout << "delete " << keys.size()/2 << " keys in " << timer.elapsed() << " seconds\n";

    //test get after delete
    timer.reset();
    timer.start();
    for(size_t i = 0; i < keys.size(); ++i) {
        auto response = c.get(keys[i], status);
        if(i % 2 == 0)
            assert(status == message_tag::NOT_FOUND && response.empty());
        else
            assert(status == message_tag::OK && response.size() == 256);
    }
    timer.stop();
    std::cout << "get " << keys.size() << " keys in " << timer.elapsed() << " seconds\n";

    //test scan after delete
    {
        timer.reset();
        timer.start();
        auto pairs = c.scan(keys.front(), keys.back(), status);
        timer.stop();
        assert(status == message_tag::OK && pairs.size() == keys.size() / 2);
        std::cout << "scan " << pairs.size() <<" keys in " << timer.elapsed() << " seconds\n" ;
    }
    std::cout << "Passed all tests!\n";


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

        run_testcase_1(host);
        /*
        std::vector<std::thread> threads;
        for (int i = 0; i < 10; ++i) {
            threads.push_back(std::thread(run_client, i));
        }
        for (auto &th : threads)
            th.join();
*/
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}