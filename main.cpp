
#include "include/Server.hpp"



int main(int ac, char** av) {
    if (ac != 3) {
        // Print error message in red
        std::cerr << RED << "Error: " << RESET << "Expected 2 arguments, but got " << ac - 1 << std::endl;
        std::cerr << YELLOW << "Usage: " << RESET  << "./ircserv <port> <password>" << std::endl;
        std::cerr << "port: The listening port" << std::endl;
        std::cerr << "password: The connection password" << std::endl;
        return 1;
    }
	try 
    {
        Server Server(av);
        Server.setup();
    }
    catch (const std::exception& e)
    {
        //clean
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}

