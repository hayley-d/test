#include <gtest/gtest.h>
#include "server/server.h"
#include "QueryHandler.h"
#include <pistache/endpoint.h>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <stdexcept>
#include <array>
#include <memory>
#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using namespace Pistache;
class ServerTest : public ::testing::Test {
protected:
    static Http::Endpoint* apiServer;
    static std::thread* serverThread;

    static void SetUpTestSuite() {
        Address addr(Ipv4::any(), Port(9080));
        apiServer = new Http::Endpoint(addr);
        auto opts = Http::Endpoint::options().threads(1);
        apiServer->init(opts);
        apiServer->setHandler(Http::make_handler<QueryHandler>());
        serverThread = new std::thread([](){
            apiServer->serve();
        });
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }


    static std::string exec(const char* cmd) {
        std::array<char, 128> buffer;
        std::string result;
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
            result += buffer.data();
        }
        return result;
    }

    static std::string runCurl(const std::string& payload, const std::string& method = "POST") {
        std::string command = "curl -s -X " + method + " http://localhost:9080 ";
        if(method == "POST") {
            command += "-H \"Content-Type: application/json\" -d '" + payload + "'";
        }
        return exec(command.c_str());
    }
};

Http::Endpoint* ServerTest::apiServer = nullptr;
std::thread* ServerTest::serverThread = nullptr;


TEST_F(ServerTest, GetTable) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "GET TABLE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser retrieved records from TABLE.", jsonRes["message"].get<std::string>());
    EXPECT_TRUE(jsonRes["data"].is_array());
}

TEST_F(ServerTest, GetDatabase) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "GET DATABASE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser retrieved records from DATABASE.", jsonRes["message"].get<std::string>());
    EXPECT_TRUE(jsonRes["data"].is_array());
}

TEST_F(ServerTest, GetRecords) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "GET RECORDS"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser retrieved records from RECORDS.", jsonRes["message"].get<std::string>());
    EXPECT_TRUE(jsonRes["data"].is_array());
}


TEST_F(ServerTest, SetDatabase) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "SET DATABASE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser created a new database.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, SetTable) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "SET TABLE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser created a new table.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, SetRecord) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "SET RECORDS"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser created a new record in RECORDS.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, UpdateDatabase) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "UPDATE DATABASE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser updated a record in DATABASE.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, UpdateTable) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "UPDATE TABLE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser updated a record in TABLE.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, UpdateRecord) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "UPDATE RECORDS"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser updated a record in RECORDS.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, DeleteDatabase) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "DELETE DATABASE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser deleted records from DATABASE.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, DeleteTable) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "DELETE TABLE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser deleted records from TABLE.", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, DeleteRecord) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "DELETE RECORDS"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_TRUE(jsonRes["success"].get<bool>());
    EXPECT_EQ("testUser deleted records from RECORDS.", jsonRes["message"].get<std::string>());
}


TEST_F(ServerTest, MissingParameters) {
    std::string payload = R"({"userID": "testUser", "query": "GET TABLE"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_FALSE(jsonRes["success"].get<bool>());
    EXPECT_EQ("Missing required parameters", jsonRes["message"].get<std::string>());
}

TEST_F(ServerTest, InvalidJSON) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "INVALID JSON" )";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_FALSE(jsonRes["success"].get<bool>());
    EXPECT_EQ("Invalid JSON format", jsonRes["message"].get<std::string>());
    EXPECT_EQ(payload, jsonRes["data"].get<std::string>());
}

TEST_F(ServerTest, InvalidOperation) {
    std::string payload = R"({"userID": "testUser", "server_secret": "secret", "query": "FOO BAR"})";
    std::string response = runCurl(payload);
    json jsonRes = json::parse(response);
    EXPECT_FALSE(jsonRes["success"].get<bool>());
    EXPECT_EQ("Invalid operation", jsonRes["message"].get<std::string>());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    // After tests finish, keep the server running.
    std::cout << "All tests finished. Server is still running on port 9080." << std::endl;
    std::cout << "Press ENTER to shut down the server and exit." << std::endl;
    std::cin.get();
    return result;
}
