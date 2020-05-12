//
// Created by wxu on 12.05.20.
//
#include <iostream>
#include <thread>
#include "client_behaviors.h"


int main(int argc, char *argv[]) {
    try {
        std::string host;
        if (argc == 1) {
            std::cout << "Connect to localhost\n";
            host = "localhost";
        } else {
            host = std::string(argv[1]);
        }

    std::vector<std::thread> threads;
    std::vector<double> time_elapsed;
    for (int i = 0; i < 5; ++i) {
        time_elapsed.push_back(0.0);
        threads.push_back(std::thread(benign_client, std::ref(host), i, std::ref(time_elapsed[i])));
    }
    for (auto &th : threads)
        th.join();
    double average = 0.0;
    for (double d : time_elapsed)
        average += d;
    average /= time_elapsed.size();
    std::cout << time_elapsed.size() << " clients takes " << average << " seconds in average."
              << "About 65.000 OPs per client.\n";


        std::cout << "Passed all tests in scenario 2\n";
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}