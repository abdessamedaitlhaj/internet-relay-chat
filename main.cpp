
#include "include/Server.hpp"


void leaks() {
    system("leaks ircserv");
    system("lsof -c ircserv");
}

int main(int ac, char** av) {

    // atexit(leaks);
    if (ac != 3) {
        std::cerr << RED << "Error: " << RESET << "Expected 2 arguments, but got " << ac - 1 << std::endl;
        std::cerr << YELLOW << "Usage: " << RESET  << "./ircserv <port> <password>" << std::endl;
        std::cerr << "port: The listening port" << std::endl;
        std::cerr << "password: The connection password" << std::endl;
        return 1;
    }
	try {
        Server Server(av);
        Server.setup();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
