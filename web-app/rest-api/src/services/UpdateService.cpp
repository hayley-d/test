#include "UpdateService.h"
#include <nlohmann/json.hpp>
#include <string>
#include <exception>

using json = nlohmann::json;


//at this point validation has been done in the QueryHandler.cpp
void UpdateService::updateType(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    //implement logic to choose correct update operation
    //first need to discuss the update operations that we will have

}

void UpdateService::updateRecord(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response){

    try{

        auto body = request.body();
        auto requestBody = json::parse(body);

        //make use of existing daemon classes to implement the rest

        

    } catch(std::exception &e){

        std::cerr << "Exception: " << e.what() << std::endl;
        response.send(Pistache::Http::Code::Internal_Server_Error, e.what());

    }

}

