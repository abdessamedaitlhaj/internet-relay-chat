#include "bot.hpp"



Bot::Bot(char **av) {
    _log = false;

	std::string port(av[1]);
    std::string password(av[2]);
	std::string  nick(av[3]);
    _port = parse_port(port);
    _password = parse_password(password);
	_nick = parse_nick(nick);
}

std::string Bot::parse_password(std::string password)
{
	if (!password.empty()  && std::isspace(password.at(0)) )
		throw std::invalid_argument("Error: Password must not start with a whitespace character.");
	return(password);
}


std::string Bot::parse_nick(std::string nick)
{
	if (!nick.empty()  && std::isspace(nick.at(0)) )
		throw std::invalid_argument("Error: nick must not start with a whitespace character.");
	return(nick);
}

Bot::~Bot() {
}



int Bot::parse_port(std::string port)
{
    if (port.empty()) {
        throw std::invalid_argument("Port cannot be empty.");
    }

    for (size_t i = 0; i < port.length(); ++i) {
        if (!std::isdigit(port[i])) {
            throw std::invalid_argument("Port contains non-numeric characters.");
        }
    }
    int parsedPort = std::atoi(port.c_str());

    if (parsedPort <= 0 || parsedPort > 65535) {
        throw std::out_of_range("Port number must be between 1 and 65535.");
    }

    // exclude well known ports 0 --> 1023
    if (parsedPort >= 0 && parsedPort <= 1023) {
        throw std::invalid_argument("Port is a reserved, well-known port.");
    }

    return parsedPort;
}

void Bot::sendResponse(int fd, const std::string& response) {
    if (send(fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client " << fd << std::endl;
    }
}

void handle(int signum){
	(void)signum;
	std::cout << std::endl << "Signal received !!" << std::endl;
    // maybe send quit to the server if they handled it 
    exit(1);
}


void Bot::setup()
{
    signal(SIGINT, handle);
	signal(SIGQUIT, handle);
    

    struct sockaddr_in ircServerAddr;
	botsock = socket(AF_INET, SOCK_STREAM, 0);
	if (botsock == -1)
	{
    std::cerr << "failed to create socket !!" << std::endl;
    return ;
    }

    ircServerAddr.sin_family = AF_INET;
    ircServerAddr.sin_port = htons(_port);
	ircServerAddr.sin_addr.s_addr = inet_addr(LOCALHOST);
	
    if(connect(botsock, (struct sockaddr*)&ircServerAddr, sizeof(ircServerAddr)) == -1)
	{
        std::cerr << "connect() failed"  << std::endl; 
        return ;
    }

    sendResponse(botsock, "PASS " + _password + "\r\n");
    sendResponse(botsock, "NICK " + _nick + "\r\n");
    std::string name(USERNAME);
    sendResponse(botsock, "USER " + name + "\r\n");

    game();
}


std::string Bot::extractNickname(std::string& message) {
    size_t start = message.find(":");
    size_t end = message.find("!");

    if (start == std::string::npos || end == std::string::npos || start >= end)
        return ""; // Return empty string if parsing fails

    return message.substr(start + 1, end - start - 1);
}

void Bot::game() {
	char buff[1024];
	while(true)
	{
		memset(buff, 0, sizeof(buff));
		ssize_t Bytenumber = recv(botsock, buff, sizeof(buff) - 1, 0);
		if(Bytenumber < 1)
        {
			break;
        }
		std::string retrieved = buff;
		size_t pos = retrieved.find_first_of("\n\r");
		if(pos != std::string::npos)
			retrieved = retrieved.substr(0, pos);
		if(retrieved == ": 001 " + _nick + " :Welcome to the Internet Relay Network "+ _nick + "!~bot@127.0.0.1" && !_log)
			_log = true;
		else if (!_log){
			std::cout << retrieved << std::endl;
			std::cout << _log << std::endl;
			return;
		}
		else if(retrieved.find("PRIVMSG") != std::string::npos && _log)
		{
            std::string sender_nick = extractNickname(retrieved);
            std::cout << retrieved  << std::endl;
            if(gameMap.find(sender_nick) == gameMap.end()) {
                gameMap[sender_nick] = QuestionGame();  // Create new game object if it doesn't exist
            }

            play(gameMap[sender_nick], sender_nick);
		}
	}
}

void Bot::play(QuestionGame& game, std::string& UserNick)
{
    std::vector<std::string> tokens;  // You need to populate this with input tokens (e.g., from the user message)
    std::string priv = "PRIVMSG " + UserNick + " :";
    std::string response = MENU_MESSAGE;

    // Ensure correct number of tokens
    std::cout << tokens.size() << std::endl;
    if (tokens.size() != 3)
        return sendResponse(botsock, priv + response);

    // Check if it's a valid game-related command
    bool tokenGame = (tokens[2] == "start" || tokens[2] == "a" || tokens[2] == "b" || tokens[2] == "c" || tokens[2] == "d");
        std::cout <<  "lol\n";
    if (!tokenGame)
        return sendResponse(botsock, priv + response);  // Only process valid game tokens

    // Start the game
    if (tokens[2] == "start") {
        if (game.getStarted()) {
            sendResponse(botsock, priv + "The game has already started!\n");
            return;
        }
        sendResponse(botsock, priv + WELCOME_MESSAGE);
        game.setStarted(true);  // Mark the game as started
        game.setQuestionSent(false);  // No question has been sent yet

        // Send the first question immediately after the game starts
        QuestionGame::Question q = game.getRandomQuestion(game.getLevel());
        sendResponse(botsock, priv + q.problem);  // Send the question to the user
        game.setAnswer(q.correct);  // Set the correct answer for this question
        game.setQuestionSent(true);  // Mark that the question has been sent

        return;
    }

    // Process the answer choices: "a", "b", "c", "d"
    if (tokens[3] == "a" || tokens[3] == "b" || tokens[3] == "c" || tokens[3] == "d") {
        if (!game.isQuestionSent()) {
            sendResponse(botsock, priv + "You need to start the game first!\n");
            return;
        }

        if (tokens[3].at(0) == game.getAnswer()) {  // Check if the answer matches the correct one
            // Correct answer handling
            switch (game.getLevel()) {
                case 1:
                    sendResponse(botsock, priv + LVL1_MESSAGE);
                    break;
                case 2:
                    sendResponse(botsock, priv + LVL2_MESSAGE);
                    break;
                case 3:
                    sendResponse(botsock, priv + LVL3_MESSAGE);
                    break;
                case 4:
                    sendResponse(botsock, priv + LVL4_MESSAGE);
                    break;
                default:
                    sendResponse(botsock, priv + "You won the game!");
                    break;
            }
            game.addLevel();  // Increase level after correct answer
            game.setQuestionSent(false);  // Reset question sent flag
        } else {
            // Incorrect answer handling
            sendResponse(botsock, priv + END_MESSAGE);
            game.setLevel(1);  // Reset to level 1
            game.setStarted(false);  // Game ends, so reset started flag
            game.setQuestionSent(false);  // Reset question sent flag
        }
        return;
    }
}
#include "bot.hpp"

int main (int ac, char**av)
{
	if (ac != 4) {
        // Print error message in red
        std::cerr << RED << "Error: " << RESET << "Expected 3 arguments, but got " << ac - 1 << std::endl;
        std::cerr << YELLOW << "Usage: " << RESET  << "./ircserv <port> <password> <Nickname>" << std::endl;
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
}#include "question.hpp"


QuestionGame::QuestionGame() : level(1), started(false), questionSent(false), correctAnswer(' ')
{
		initializeLevel1();
		initializeLevel2();
		initializeLevel3();
		initializeLevel4();
}

void QuestionGame::initializeLevel1() {
            std::vector<Question> level1;
            level1.push_back(Question());
            level1[0].problem = Q11;
            level1[0].correct = 'c';
            
            level1.push_back(Question());
            level1[1].problem = Q12;
            level1[1].correct = 'b';

            level1.push_back(Question());
            level1[2].problem = Q13;
            level1[2].correct = 'b';

            level1.push_back(Question());
            level1[3].problem = Q14;
            level1[3].correct = 'a';

            level1.push_back(Question());
            level1[4].problem = Q15;
            level1[4].correct = 'b';

            levels.push_back(level1);
}

void QuestionGame::initializeLevel2() {
	        std::vector<Question> level2;
            level2.push_back(Question());
            level2[0].problem = Q21;
            level2[0].correct = 'a';
            
            level2.push_back(Question());
            level2[1].problem = Q22;
            level2[1].correct = 'b';

            level2.push_back(Question());
            level2[2].problem = Q23;
            level2[2].correct = 'c';

            level2.push_back(Question());
            level2[3].problem = Q24;
            level2[3].correct = 'b';

            level2.push_back(Question());
            level2[4].problem = Q25;
            level2[4].correct = 'a';

            levels.push_back(level2);
}

void QuestionGame::initializeLevel3() {

	    std::vector<Question> level3;
        level3.push_back(Question());
        level3[0].problem = Q31;
        level3[0].correct = 'c';

		level3.push_back(Question());
        level3[1].problem = Q32;
        level3[1].correct = 'a';

		level3.push_back(Question());
        level3[2].problem = Q33;
        level3[2].correct = 'c';

        level3.push_back(Question());
        level3[3].problem = Q34;
        level3[3].correct = 'a';

    	level3.push_back(Question());
        level3[4].problem = Q35;
        level3[4].correct = 'a';

        levels.push_back(level3);
}

void QuestionGame::initializeLevel4() {

		std::vector<Question> level4;
        level4.push_back(Question());
        level4[0].problem = Q41;
        level4[0].correct = 'a';

		level4.push_back(Question());
        level4[1].problem = Q42;
        level4[1].correct = 'a';

		level4.push_back(Question());
        level4[2].problem = Q43;
        level4[2].correct = 'a';

        level4.push_back(Question());
        level4[3].problem = Q44;
        level4[3].correct = 'b';

    	level4.push_back(Question());
        level4[4].problem = Q45;
        level4[4].correct = 'b';

        levels.push_back(level4);



}


QuestionGame::Question &QuestionGame::getRandomQuestion(int level) {
    srand(time(0));
    int index = rand() % levels[level - 1].size();
    return levels[level - 1][index];
}

std::string QuestionGame::gameMessage(int flag)
{
    if (flag == MENU)
        return MENU_MESSAGE;
    
    if (flag == END)
        return END_MESSAGE;
    
    if (flag == WELCOME)
        return WELCOME_MESSAGE;
    
    if (flag == LVL1)
        return LVL1_MESSAGE;
    
    if (flag == LVL2)
        return LVL2_MESSAGE;
    
    if (flag == LVL3)
        return LVL3_MESSAGE;
    
    if (flag == LVL4)
        return LVL4_MESSAGE;
    
	return (END_MESSAGE);
}

