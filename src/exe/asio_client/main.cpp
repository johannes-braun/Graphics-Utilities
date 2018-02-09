#include <array>
#include <iostream>
#include <asio/asio.hpp>

using asio::ip::tcp;

int main()
{
    try
    {
        asio::io_service service;
        tcp::resolver resolver(service);

        tcp::resolver::query query("localhost", "13");
        tcp::resolver::iterator endpoints = resolver.resolve(query);
        

        for (;;)
        {
            tcp::socket socket(service);
            asio::connect(socket, endpoints);

            for (;;)
            {
                std::array<char, 128> buffer;
                asio::error_code error;
                size_t len = socket.read_some(asio::buffer(buffer), error);

                if (error == asio::error::eof)
                    break;
                if (error)
                    throw asio::system_error(error);

                std::cout.write(buffer.data(), len);
            }
        }
    } catch(std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    system("pause");
    return 0;
}