//
// Created by wxu on 08.05.20.
//

//#define ASIO_STANDALONE
//#define ASIO_DISABLE_THREADS

#include <iostream>
#include <asio/detail/array.hpp>
#include <asio.hpp>
#include <thread>
#include <message.h>

using asio::ip::tcp;
asio::io_context io_context;
char* host;
void mysend(int id)
{
   tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(host, "1313");
        tcp::socket socket(io_context);
        for(int i = 0; i < 100; ++i) {
            asio::connect(socket, endpoints);

            std::string msg(265, 'a' + i % 26);
            msg[0] = message::PUT;
            msg[1] = 'A' + id %26;
            msg[2] = 'A' + (id / 26) % 26;
            msg[3] = 'A' + i % 26;
            msg[4] = 'A' + (i / 26) % 26;
            msg[264] = 'A' + i % 26;
            msg[263] = 'A' + (i / 26) % 26;
            asio::error_code ignored;
            asio::write(socket, asio::buffer(msg), ignored);

            for (;;) {
                asio::detail::array<char, 128> buf;
                asio::error_code error;
                size_t len = socket.read_some(asio::buffer(buf), error);
                if (error == asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw asio::system_error(error); // Some other error.

                std::cout << id << " : ";
                std::cout.write(buf.data(), len);
            }
            std::cout << "\n";

        }

        for(int i = 0; i < 1; ++i) {
            asio::connect(socket, endpoints);

            std::string msg(17, 'a' + i % 26);
            msg[0] = message::SCAN;
            for(int offset = 1; offset < 9; ++offset) {
                msg[offset] = 0;
                msg[offset + 8] = 127;
            }
            asio::error_code ignored;
            asio::write(socket, asio::buffer(msg), ignored);
            std::cout << id << " received: ";
            int l = 0;
            for (;;) {
                asio::detail::array<char, 128> buf;
                asio::error_code error;
                size_t len = socket.read_some(asio::buffer(buf), error);
                l += len;
                if (error == asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw asio::system_error(error); // Some other error.

                std::cout.write(buf.data(), len);
            }
            std::cout << "\n" << l << "\n";
        }
}
int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }
        host = argv[1];
        std::vector<std::thread> threads;
        for(int i = 1; i <= 1; ++i){
            threads.push_back(std::thread(mysend, i));
        }

        for(auto& th : threads)
            th.join();

    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
