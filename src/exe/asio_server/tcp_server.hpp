#pragma once

#include <asio.hpp>

namespace net
{
    using asio::ip::tcp;

    class tcp_server
    {
    public:
        tcp_server(asio::io_service& service)
            : _acceptor(service, tcp::endpoint(tcp::v4(), 13)), _socket(service)
        {
            start_accept();
        }


    private:
        void start_accept()
        {
            tlog_i("TCP Server") << "Listen for socket connection...";
            _acceptor.async_accept(_socket, [this](asio::error_code error)
            {
                tlog_i("TCP Server") << "Accept socket connection.";
                asio::error_code msg_error;
                constexpr const char delim[] = "\n\r\n\r";
                asio::async_read_until(_socket, _read_buffer, delim, [this, error](asio::error_code err, size_t len)
                {
                    if(len==0)
                        return _socket.close(), start_accept();

                    std::string msg{
                        asio::buffers_begin(_read_buffer.data()),
                        asio::buffers_begin(_read_buffer.data()) + len
                        - sizeof(delim) + 1 };
                    _read_buffer.consume(len);
                    if(msg.substr(0, 5) != "Hello")
                    {
                        tlog_w("TCP Server") << "A client was not nice and said \"" << msg << "\", so connection is being refused.";
                        _message = "I won't talk to you.";
                        asio::write(_socket, asio::buffer(_message));
                        return _socket.close(), start_accept();
                    }

                    tlog_i("TCP Server") << "A nice client said \"" << msg << "\". Sending acknowledgement...";
                    _message = "Hi, I am here.";
                    asio::write(_socket, asio::buffer(_message));
                    return _socket.close(), start_accept();
                });
            });
        }

        asio::streambuf _read_buffer;
        tcp::acceptor _acceptor;
        tcp::socket _socket;
        std::string _message;
    };
}
