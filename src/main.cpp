
#include "../include/server.hpp"



int main(int ac, char** av) {
    // Check if the correct number of arguments is provided
    if (ac != 3) {
        // Print error message in red
        std::cerr << RED << "Error: " << RESET << "Expected 2 arguments, but got " << ac - 1 << ".\n";
        std::cerr << YELLOW << "Usage: " << RESET  << "./ircserv <port> <password>" << std::endl;
        std::cerr << "port: The listening port" << std::endl;
        std::cerr << "password: The connection password" << std::endl;
        return 1;
    }

    
    return 0;
}