#include "HTTPServer.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
using json = nlohmann::json;

// Setup io_context and port number to use for server
HTTPServer::HTTPServer(int portNumber) : io_context(), acceptor(io_context, {tcp::v4(), static_cast<unsigned short>(portNumber)}), running(true)
{   
    if(portNumber < 1023)
    {
        std::cout << "Port up to 1023 are reserved: Opening server on port 9005\n";
        this->portNumber = 9005;
    }
    else
    {   
        this->portNumber = portNumber;
    }

}

// Returns value of key for file filename if found. Otherwise return empty string
std::string HTTPServer::getEnvironmentKey(const std::string filename, const std::string key)
{
    std::ifstream env_file(filename);
    if (!env_file.is_open())
    {
        throw std::runtime_error("The environment file could not be loaded");
    }

    std::string fileLine;
    while (std::getline(env_file, fileLine))
    {
        // Trim leading/trailing whitespace
        fileLine.erase(fileLine.find_last_not_of(" \t\r\n") + 1);
        fileLine.erase(0, fileLine.find_first_not_of(" \t\r\n"));

        // Check if the line starts with the key and contains '='
        if (fileLine.rfind(key + "=", 0) == 0) // Ensure key is at the start
        {
            size_t posOfKey = fileLine.find("=");
            if (posOfKey == std::string::npos) continue; // Skip invalid lines

            std::string value = fileLine.substr(posOfKey + 1);
            // Trim whitespace from value
            value.erase(value.find_last_not_of(" \t\r\n") + 1);
            value.erase(0, value.find_first_not_of(" \t\r\n"));

            return value;
        }
    }

    env_file.close();
    return ""; // Return empty if key is not found
}


void HTTPServer::acceptConnections()
{
    auto socket = std::make_shared<tcp::socket>(io_context);
    acceptor.async_accept(*socket, [this, socket](boost::system::error_code e)
    {
        if (!e && running) 
        {
            handleSession(socket);
        }
        if (running) 
        {
            acceptConnections(); // Accept the next connection
        }
    });
}

void HTTPServer::runServer()
{
    this->acceptConnections();
    io_context.run();
}

// Create thread running runServer function to listen for incoming requests
void HTTPServer::start()
{
    server_thread = std::thread([this] {runServer(); }); //Note the [this] syntax is for creating a lambda function in c++
}

// Gracefully terminates server and joins threads
void HTTPServer::stop()
{
    running = false;
    acceptor.cancel();
    io_context.stop();
    if(server_thread.joinable())
    {
        server_thread.join();
    }
}

void HTTPServer::handleSession(std::shared_ptr<tcp::socket> socket)
{
    auto buffer = std::make_shared<boost::beast::flat_buffer>();
    auto request = std::make_shared<http::request<http::string_body>>();

    boost::beast::http::async_read(*socket, *buffer, *request,
        [this, socket, buffer, request](boost::system::error_code e, std::size_t)
        {
            if (!e)
            {
                if (request->method() == http::verb::post && request->target() == "/QRY") 
                {
                    handlePostRequest(*request, socket);
                } 
                else 
                {
                    handleUnknownRequest(*request, socket);
                }
            }
            socket->shutdown(tcp::socket::shutdown_send);
        }
    );
}

void HTTPServer::handlePostRequest(http::request<http::string_body> &request, std::shared_ptr<tcp::socket> socket)
{
    // Prepare response message and set headers
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "MPDB DB Server");
    response.set(http::field::content_type, "application/json");
    json response_json;

    // Load server secret from .env
    std::string server_secret = getEnvironmentKey("environment_files/server_info.txt", "SERVER_SECRET");
    if(server_secret != "")
    {
        
        // Authenticate server secret
        json request_body= json::parse(request.body());
        if(request_body["server_secret"] == server_secret)
        {
            // Handle request
            response_json["success"] = true;
            response_json["message"] = "200: Successfully connected to server";
            response_json["data"] = "";            
        }
        else
        {
            response_json["success"] = false;
            response_json["message"] = "401: Server secret is incorrect";
            response_json["data"] = "";
        }

    } 
    else
    {
        response_json["success"] = false;
        response_json["message"] = "The server could not find the server secret. Please contact administrator";
        response_json["data"] = "";
    }

    // Return response
    response.body() = response_json.dump();
    response.prepare_payload();
    boost::beast::http::write(*socket, response);
}

void HTTPServer::handleUnknownRequest(http::request<http::string_body> &request, std::shared_ptr<tcp::socket> socket)
{
    http::response<http::string_body> response;
    response.version(request.version());
    response.result(http::status::ok);
    response.set(http::field::server, "MPDB DB Server");
    response.set(http::field::content_type, "application/json");
    json response_json;

    response_json["success"] = false;
    response_json["message"] = "404: Endpoint does not exist"; 
    response_json["data"] = "";
    
    response.body() = response_json.dump();
    response.prepare_payload();
    boost::beast::http::write(*socket, response);
}
