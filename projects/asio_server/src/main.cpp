#include <array>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iostream>
#include <string>
#include <asio.hpp>
#include <jpu/log.hpp>
#include "tcp_server.hpp"

using asio::ip::tcp;

int main() try
{
    asio::io_service service;
    net::tcp_server server(service);
    service.run();
} 
catch (std::exception& e)
{
    tlog_e("Server") << e.what();
    system("pause");
}




int main_02() try
{
    asio::io_service service;
    tcp::acceptor acceptor(service, tcp::endpoint(tcp::v4(), 13));

    for(;;)
    {
        tcp::socket socket(service);
        acceptor.accept(socket);

        std::array<char, 128> buffer;
        asio::error_code error;
        size_t len = socket.read_some(asio::buffer(buffer), error);

        if (error == asio::error::eof)
            break;
        if (error)
            throw asio::system_error(error);

        std::stringstream message_buffer;
        message_buffer.write(buffer.data(), len);
        tlog_i("Server") << "Received Message: " << message_buffer.str();

        std::string message = "Yep I am here.";
        asio::error_code ignored_error;
        asio::write(socket, asio::buffer(message), ignored_error);
    }
    return 0;
} 
catch(std::exception& e)
{
    std::cerr << e.what() << '\n';
    system("pause");
    return 1;
}