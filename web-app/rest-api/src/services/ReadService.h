#ifndef READSERVICE_H
#define READSERVICE_H

#include <pistache/router.h>

class ReadService{

    public:

        //function that finds out what needs to be readT
        static void readType(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

        //gets redirected to different create functions for different database entities
        static void readRecord(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
        static void readDatabase(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);
        static void readTable(const Pistache::Rest::Request& request, Pistache::Http::ResponseWriter response);

};


#endif //READSERVICE_H