#include "server/server.h"
#include <pistache/endpoint.h>

using namespace Pistache;

int main(){

    Pistache::Address addr(Pistache::Ipv4::any(), Pistache::Port(9080));
    Http::Endpoint apiServer(addr);

    auto opts = Http::Endpoint::options().threads(1);
    apiServer.init(opts);
    apiServer.setHandler(Http::make_handler<QueryHandler>());
    apiServer.serve();

    apiServer.shutdown();
    return 0;
}
