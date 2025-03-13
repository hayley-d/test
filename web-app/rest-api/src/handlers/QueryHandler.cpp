#include "QueryHandler.h"
#include "../vendor/json.hpp"
#include <string>
#include <exception>
#include <sstream>

#include "../services/CreateService.h"

using json = nlohmann::json;
using namespace Pistache;

void QueryHandler::onRequest(const Http::Request& request, Http::ResponseWriter response)
{
    try{

        auto body = request.body();
        auto j = json::parse(body);

        // Validate required parameters.
        if (!j.contains("userID") || !j.contains("server_secret") || !j.contains("query")) {
            json res = {
                {"success", false},
                {"message", "Missing required parameters"},
                {"data", json::array()}
            };
            response.send(Http::Code::Bad_Request, res.dump());
            return;
        }

        std::string userID = j["userID"];
        std::string query = j["query"];

        // Parse the query using a string stream.
        std::istringstream iss(query);
        std::string op, collection;
        iss >> op >> collection;

        //No validation done at this point
        if(op == "SET"){

            if(collection == "DATABASE"){

                std::cout << "Creating a new database." << std::endl;

                json res = {
                    {"success", true},
                    {"message", userID + " created a new database."}
                };

                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Created, res.dump());
                return;

            } else if(collection == "TABLE"){

                std::cout << "Creating a new table." << std::endl;

                json res = {
                    {"success", true},
                    {"message", userID + " created a new table."}
                };

                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Created, res.dump());
                return;

            } else{

                std::cout << "Creating a new record in collection: " << collection << std::endl;

                json res = {
                    {"success", true},
                    {"message", userID + " created a new record in " + collection + "."}
                };

                response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
                response.send(Http::Code::Created, res.dump());
                return;

            }
        } else if(op == "UPDATE"){

            if(collection == "DATABASE" || collection == "TABLE"){

                //implement for special cases

            }

            std::cout << "Updating a record in collection: " << collection << std::endl;

            json res = {
                {"success", true},
                {"message", userID + " updated a record in " + collection + "."}
            };

            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Ok, res.dump());
            return;

        } else if(op == "GET"){

            std::cout << "Retrieving from collection: " << collection << std::endl;

            json res = {
                {"success", true},
                {"message", userID + " retrieved records from " + collection + "."},
                {"data", json::array()}
            };

            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Ok, res.dump());
            return;

        } else if(op == "DELETE"){

            std::cout << "Deleting from collection: " << collection << std::endl;

            json res = {
                {"success", true},
                {"message", userID + " deleted records from " + collection + "."}
            };

            response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
            response.send(Http::Code::Ok, res.dump());
            return;

        } else{

            json res = {
                {"success", false},
                {"message", "Invalid operation"},
                {"data", json::array()}
            };

            response.send(Http::Code::Bad_Request, res.dump());
            return;

        }

    } catch(std::exception &e){

        json res = {
            {"success", false},
            {"message", "Invalid JSON format"},
            {"data", request.body()}
        };

        response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
        response.send(Http::Code::Bad_Request, res.dump());

    }
    
}