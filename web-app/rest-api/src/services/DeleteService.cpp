#include "DeleteService.h"
#include <nlohmann/json.hpp>
#include <string>
#include <exception>

using json = nlohmann::json;


//at this point validation has been done in the QueryHandler.cpp
void DeleteService::deleteType(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void DeleteService::deleteRecord(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void DeleteService::deleteDatabase(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

void DeleteService::deleteTable(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}
