#ifndef SERVER_H
#define SERVER_H

#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <memory>

#include "../handlers/QueryHandler.h"

using namespace Pistache;

class server{

    public:
        explicit server(Pistache::Address address); //use explicit keyword to avoid automatic type conversion avoiding issues
        ~server();

        void init(size_t numThreads = 1);
        void start();
        void shutdown();
        

    private:
        std::shared_ptr<Pistache::Http::Endpoint> httpEndpoint; //Manages HTTP server that listens for incoming requests
        std::shared_ptr<Pistache::Rest::Router> router; //Maps HTTP endpoints

        

};

#endif
