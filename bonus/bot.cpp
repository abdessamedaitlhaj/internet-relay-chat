#include "bot.hpp"

int botsock;

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
    for (size_t i = 0; i < password.length(); ++i) {
        if (std::isspace(password[i]) || !std::isprint(password[i])) {
            throw std::invalid_argument("Error: Password contains invalid characters.");
        }
    }
	return(password);
}


std::string Bot::parse_nick(std::string nick)
{
    if (nick.empty())
        throw std::invalid_argument("Error: Nickname cannot be empty.");
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

void sendResponse(int fd, const std::string& response) {
    if (send(fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client " << fd << std::endl;
    }
}

void handle(int signum){
	(void)signum;
	std::cout << std::endl << "Signal received !!" << std::endl;
    // maybe send quit to the server if they handled it 
    sendResponse(botsock, "QUIT " "\r\n");
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
			break;
		_buffer.append(buff, Bytenumber);
        size_t pos;
        while ((pos = _buffer.find("\r\n")) != std::string::npos)
        {
            std::string retrieved = _buffer.substr(0, pos);
            _buffer.erase(0, pos + 2);
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

                play(gameMap[sender_nick], sender_nick, retrieved);
		    }
        }
	}
}

void Bot::privatemessage(std::string& UserNick, std::string message) {
    std::string priv = "PRIVMSG " + UserNick + " :";
    std::istringstream stream(message);
    std::string line;

    while (std::getline(stream,  line, '\n')) {
        sendResponse(botsock, priv + line + "\n");
    }
}

void Bot::play(QuestionGame& game, std::string& UserNick, std::string retrieved)
{
    std::string response = MENU_MESSAGE;
    std::vector<std::string> tokens;
    std::istringstream stream(retrieved);
    std::string token;

    while (stream >> token) {  // Automatically splits by spaces or tabs
        tokens.push_back(token);
    }

    if (tokens.size() != 4)
        return privatemessage(UserNick, response);

    // Check if it's a valid game-related command
    bool start = (tokens[3] == "start" || tokens[3] == ":start");
    bool charGame = ( tokens[3] == "a" || tokens[3] == "b" || tokens[3] == "c" || tokens[3] == "d");
    bool charSemi = (tokens[3] == ":a" || tokens[3] == ":b" || tokens[3] == ":c" || tokens[3] == ":d") ;
    bool tokenGame = start || charGame || charSemi;
    if (!tokenGame)
        return privatemessage(UserNick, response);  // Only process valid game tokens

    // Start the game
    if (start) {
        if (game.getStarted()) {
            privatemessage(UserNick, "The game has already started!\n");
            return;
        }
        privatemessage(UserNick,+ WELCOME_MESSAGE);
        game.setStarted(true);  // Mark the game as started
        game.setQuestionSent(false);  // No question has been sent yet

        // Send the first question immediately after the game starts
        QuestionGame::Question q = game.getRandomQuestion(game.getLevel());
        privatemessage(UserNick, q.problem);  // Send the question to the user
        game.setAnswer(q.correct);  // Set the correct answer for this question
        game.setQuestionSent(true);  // Mark that the question has been sent

        return;
    }

    // Process the answer choices: "a", "b", "c", "d"
    if (charGame || charSemi) {
        if (!game.isQuestionSent()) {
            privatemessage(UserNick,  "You need to start the game first!\n");
            return;
        }

        if ((charGame && tokens[3].at(0) == game.getAnswer()) || (charSemi && tokens[3].at(1) == game.getAnswer())) {  // Check if the answer matches the correct one
            // Correct answer handling
            switch (game.getLevel()) {
                case 1:
                    privatemessage(UserNick, LVL1_MESSAGE);
                    break;
                case 2:
                    privatemessage(UserNick, LVL2_MESSAGE);
                    break;
                case 3:
                    privatemessage(UserNick, LVL3_MESSAGE);
                    break;
                case 4:
                    privatemessage(UserNick, LVL4_MESSAGE);
                    break;
                default:
                    privatemessage(UserNick, "You won the game!");
                    break;
            }
            game.addLevel();  // Increase level after correct answer
            game.setQuestionSent(false); 

            if (game.getLevel() != 5)
            {
                QuestionGame::Question nextQ = game.getRandomQuestion(game.getLevel());
                privatemessage(UserNick, nextQ.problem);  // Send the next question
                game.setAnswer(nextQ.correct);  // Set the correct answer for the new question
                game.setQuestionSent(true);
            }
            else
            {
                game.setLevel(1);
                game.setStarted(false);
            }
        } else {
            // Incorrect answer handling
            privatemessage(UserNick, END_MESSAGE);
            game.setLevel(1);  // Reset to level 1
            game.setStarted(false);  // Game ends, so reset started flag
            game.setQuestionSent(false);  // Reset question sent flag
        }
        return;
    }
}
