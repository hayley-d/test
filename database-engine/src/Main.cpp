#include "HTTPServer.h"
#include <iostream>

int main()
{   
    HTTPServer server(9070);

    try
    {
        server.start();
        std::cout << "Server running...\n";

        char in;
        std::cin >> in;
        

        server.stop();
        std::cout << "Server stopped...\n";
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception " << e.what() << '\n';
    }

    return 0;
    
}
