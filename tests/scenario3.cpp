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
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        std::vector<std::thread> threads;
        std::vector<double> time_elapsed;
        for (int i = 0; i < 10; ++i) {
            if (i < 5) {
                time_elapsed.push_back(0.0);
                threads.push_back(std::thread(benign_client, std::ref(host), i, std::ref(time_elapsed[i])));
            } else if (i < 7)
                threads.push_back(std::thread(send_faulty, std::ref(host), i));
            else
                threads.push_back(std::thread(connect_and_block, std::ref(host), i));

        }
        for (auto &th : threads)
            th.join();
        double average = 0.0;
        for (double d : time_elapsed)
            average += d;
        average /= time_elapsed.size();
        std::cout << time_elapsed.size() << " clients takes " << average << " seconds in average."
                  << "About 65.000 OPs per client. Faulty clients exist.\n";

        std::cout << "Passed all tests in scenario 3\n";
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}