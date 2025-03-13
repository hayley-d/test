#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <pistache/router.h>

class UpdateService{

    public:
        static void updateType(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

        //for now I only added one update operation, we need to discuss through whether there will be more
        static void updateRecord(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
        

};

#endif //UPDATESERVICE_H