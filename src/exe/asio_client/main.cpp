#include <array>
#include <iostream>
#include <asio.hpp>
#include <jpu/log>

using asio::ip::tcp;

int main() try
{
    asio::io_service service;
    tcp::resolver resolver(service);

    tcp::resolver::query query("localhost", "13");
    tcp::resolver::iterator endpoints = resolver.resolve(query);
    tlog_i("TCP Client") << "Resolve connection to localhost:13...";

    for (;;)
    {
        tcp::socket socket(service);
        asio::connect(socket, endpoints);
        std::string message = "";
        tlog_h("TCP Client") << "Type in a nice message (nice = begins with Hello): ";
        std::getline(std::cin, message, '\n');

        asio::error_code ignored_error;
        asio::write(socket, asio::buffer(message + "\n\r\n\r"), ignored_error);

        /*for (;;)
        {*/
            std::array<char, 128> buffer;
            asio::error_code error;
            size_t len = socket.read_some(asio::buffer(buffer), error);

           /* if (error == asio::error::eof)
                break;
            if (error)
                throw asio::system_error(error);*/

            std::stringstream message_buffer;
            message_buffer.write(buffer.data(), len);
            tlog_i("TCP Client") << "Received Message: " << message_buffer.str() << '\n';
        /*}*/
    }
}
catch (std::exception& e)
{
    std::cerr << e.what() << '\n';
    system("pause");
    return 0;
}
