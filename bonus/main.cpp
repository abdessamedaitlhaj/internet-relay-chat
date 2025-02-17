#include "bot.hpp"

int main (int ac, char**av)
{
	if (ac != 4) {
        std::cerr << RED << "Error: " << RESET << "Expected 3 arguments, but got " << ac - 1 << std::endl;
        std::cerr << YELLOW << "Usage: " << RESET  << "./bot <port> <password> <Nickname>" << std::endl;
        std::cerr << "port: The server listening port" << std::endl;
        std::cerr << "password: The server connection password" << std::endl;
        std::cerr << "Nickname: The bot Nickname" << std::endl;
        return 1;
    }
	try {
        Bot bot(av);
        bot.setup();
	}
	catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}