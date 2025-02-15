#include "bot.hpp"

int main (int ac, char**av)
{
	if (ac != 4) {
        // Print error message in red
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
        //clean up
        std::cerr << e.what() << std::endl;
        return 1;
    }
	// QuestionGame q;
	    // if (q.levels.size() > 1 && q.levels[1].size() > 1) {
            //  QuestionGame::Question a = q.getRandomQuestion(4);
        // std::cout << q.levels[0][2].problem << std::endl;
        // std::cout << a.problem << std::endl;
    // } else {
    //     std::cerr << "Level 2 or the second question in level 2 does not exist" << std::endl;
    // }
}