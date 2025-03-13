#ifndef DELETESERVICE_H
#define DELETESERVICE_H

#include <pistache/router.h>

class DeleteService{

    public:

        //function that finds out what needs to be deleted
        static void deleteType(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

        //gets redirected to different delete functions for different database entities
        static void deleteRecord(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
        static void deleteDatabase(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
        static void deleteTable(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

};


#endif //DELETESERVICE_H