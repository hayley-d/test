#ifndef CREATESERVICE_H
#define CREATESERVICE_H

#include <pistache/router.h>
using namespace Pistache;

class CreateService{

    public:

        //function that finds out what needs to be created
        static void createType(const Http::Request& request, Http::ResponseWriter response);

        //gets redirected to different create functions for different database entities
        static void createRecord(const Http::Request& request, Http::ResponseWriter response);
        static void createDatabase(const Http::Request& request, Http::ResponseWriter response);
        static void createTable(const Http::Request& request, Http::ResponseWriter response);

};


#endif //CREATESERVICE_H