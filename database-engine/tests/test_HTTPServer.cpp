#include "doctest.h"
//#include <iostream>  
#include "../src/HTTPServer.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

// Helper function to make a clean request for each test
static json makeRequest(boost::asio::io_context& io_context, const tcp::resolver::results_type& results, 
                 const std::string& target, const json& requestBody) {
    // Create a fresh socket
    tcp::socket socket(io_context);
    boost::asio::connect(socket, results.begin(), results.end());
    
    // Create request
    http::request<http::string_body> request;
    request.method(http::verb::post);
    request.version(11);
    request.set(http::field::host, "127.0.0.1");
    request.set(http::field::content_type, "application/json");
    request.target(target);
    request.body() = requestBody.dump();
    request.prepare_payload();
    
    // Send the request
    http::write(socket, request);
    
    // Read response
    boost::beast::flat_buffer buffer;
    http::response<http::string_body> response;
    http::read(socket, buffer, response);
    
    // Clean up socket
    boost::system::error_code ec;
    socket.shutdown(tcp::socket::shutdown_both, ec);
    socket.close(ec);
    
    // Parse and return response
    return json::parse(response.body());
}

/*TEST_CASE("QRY Testing")
{
    // Start server in separate thread
    HTTPServer server_1(9005);
    server_1.start();
    
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto const results = resolver.resolve("127.0.0.1", "9005");
    
    SUBCASE("Make request to non-existing endpoint")
    {
        json invalidEndpointRequest;
        invalidEndpointRequest["userID"] = "xyz";
        invalidEndpointRequest["server_secret"] = "notTheActualSecret";
        invalidEndpointRequest["query"] = "Some Fake Query";
        
        json response = makeRequest(io_context, results, "/fake-endpoint", invalidEndpointRequest);
        
        json expectedResult;
        expectedResult["data"] = "";
        expectedResult["message"] = "404: Endpoint does not exist";
        expectedResult["success"] = false;
        
        CHECK(response == expectedResult);
    }
    SUBCASE("Make request with correct endpoint & wrong server secret")
    {
        json incorrectServerSecretRequest;
        incorrectServerSecretRequest["userID"] = "xyz";
        incorrectServerSecretRequest["server_secret"] = "wrongSecret";
        incorrectServerSecretRequest["query"] = "some fake secret";
        
        json response = makeRequest(io_context, results, "/QRY", incorrectServerSecretRequest);
        
        json expectedResult;
        expectedResult["data"] = "";
        expectedResult["message"] = "401: Server secret is incorrect";
        expectedResult["success"] = false;
        
        CHECK(response == expectedResult);
    }
    SUBCASE("Make request with correct endpoint & server secret")
    {
        json validRequest;
        validRequest["userID"] = "xyz";
        validRequest["server_secret"] = "oogaBooga70";
        validRequest["server_secret"] = server_1.getEnvironmentKey("environment_files/server_info.txt", "SERVER_SECRET");
        validRequest["query"] = "Some fake query";
        json response = makeRequest(io_context, results, "/QRY", validRequest);
        
        json expectedResult;
        expectedResult["data"] = "";
        expectedResult["success"] = true;
        expectedResult["message"] = "200: Successfully connected to server";
        
        CHECK(response == expectedResult);
    }
    
    server_1.stop();
}*/
