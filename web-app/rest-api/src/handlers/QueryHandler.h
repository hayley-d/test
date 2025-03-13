#ifndef QUERYHANDLER_H
#define QUERYHANDLER_H

#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/endpoint.h>

using namespace Pistache;

class QueryHandler: public Http::Handler {
public:
    HTTP_PROTOTYPE(QueryHandler)

    // Override the onRequest method.
    void onRequest(const Http::Request& request, Http::ResponseWriter response) override;

    static void create(const Http::Request& request, Http::ResponseWriter response);
    static void read(const Rest::Request &request, Http::ResponseWriter response);
    static void update(const Rest::Request &request, Http::ResponseWriter response);
    static void remove(const Rest::Request &request, Http::ResponseWriter response);

    static void createRecord(Http::ResponseWriter response);

    // New helper functions for unit testing:
    static std::pair<Http::Code, std::string> processQuery(const std::string& body);
};

#endif //QUERYHANDLER_H
