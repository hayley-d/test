#include "CreateService.h"
#include <nlohmann/json.hpp>
#include <string>
#include <exception>

using json = nlohmann::json;
using namespace Pistache;

//at this point validation has been done in the QueryHandler.cpp
void CreateService::createType(const Http::Request& request, Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        //Testing response
        nlohmann::json res = {
            {"message", "createType received"},
            {"status", "success"}
        };          
        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void CreateService::createRecord(const Http::Request& request, Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void CreateService::createDatabase(const Http::Request& request, Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void CreateService::createTable(const Http::Request& request, Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}
