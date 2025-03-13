#include "server.h"

using namespace Pistache;

server::server(Pistache::Address address){

    httpEndpoint = std::make_shared<Pistache::Http::Endpoint>(address);
    router = std::make_shared<Pistache::Rest::Router>();

}

server::~server(){

    

}

void server::init(size_t numThreads){

    auto opts = Pistache::Http::Endpoint::options().threads(numThreads);
    
    httpEndpoint->init(opts);

}

void server::start(){

    httpEndpoint->setHandler(router->handler());
    httpEndpoint->serve();

}

void server::shutdown() {
    // Calls the underlying Pistache HTTP endpoint's shutdown method.
    if(httpEndpoint) {
        httpEndpoint->shutdown();
    }
}