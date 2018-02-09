#include <ctime>
#include <iostream>
#include <string>
#include <asio/asio.hpp>

using asio::ip::tcp;

std::string make_daytime_string()
{
    std::time_t now = std::time(nullptr);
    return std::ctime(&now);
}

int main() try
{
    asio::io_service service;
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 13));

    for(;;)
    {
        tcp::socket socket(service);
        acceptor.accept(socket);

        std::string message = make_daytime_string();
        asio::error_code ignored_error;
        asio::write(socket, asio::buffer(message), ignored_error);
    }
    return 0;
} 
catch(std::exception& e)
{
    std::cerr << e.what() << '\n';
    return 1;
}