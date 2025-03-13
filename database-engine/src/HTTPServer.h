#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <boost/asio.hpp>
#include <boost/beast.hpp>

using tcp = boost::asio::ip::tcp;
namespace http = boost::beast::http;

class HTTPServer
{
    private:
        boost::asio::io_context io_context;
        tcp::acceptor acceptor;
        std::thread server_thread;
        std::atomic<bool> running;
        int portNumber;
        
        void handleSession(std::shared_ptr<tcp::socket> socket);
        void runServer();
        void handlePostRequest(http::request<http::string_body>& request, tcp::socket& socket);
        void handleUnknownRequest(http::request<http::string_body>& request, tcp::socket& socket);
        void acceptConnections();
        

    public:
        HTTPServer(int portNumber);
        ~HTTPServer() = default;
        void start();
        void stop();       
        void handlePostRequest(http::request<http::string_body>& request, std::shared_ptr<tcp::socket> socket);
        void handleUnknownRequest(http::request<http::string_body>& request, std::shared_ptr<tcp::socket> socket);
        std::string getEnvironmentKey(std::string filename, std::string key);
        
};

#endif
