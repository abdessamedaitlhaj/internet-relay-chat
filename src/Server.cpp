#include "../include/Server.hpp"
#include "../bot/question.hpp"
std::string Server::parse_password(std::string password)
{
	if (!password.empty()  && std::isspace(password.at(0)) )
		throw std::invalid_argument("Error: Password must not start with a whitespace character.");
	return(password);
}

int Server::_break = 0;

int Server::parse_port(std::string port)
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

Server::Server(char** av) {
    std::string port(av[1]);
    std::string password(av[2]);
    _port = parse_port(port);
    _password = parse_password(password);

    bot.setNickName("bot");
    bot.setUserName("bot");
    bot.setRealName("IRC Bot");
    bot.setRegistered(true);
    std::cout << "pass  :" << _password << std::endl;
    std::cout << "port  :" << _port << std::endl;
}

void Server::serverSocket() 
{
    //add comment later
    _serverAddress.sin_family = AF_INET; //ipv4
    _serverAddress.sin_port = htons(_port); // network byte order
    _serverAddress.sin_addr.s_addr = INADDR_ANY; // any local machine
    _socket = socket(AF_INET, SOCK_STREAM, 0); // server socket fd
    if(_socket == -1)
		throw(std::runtime_error("socket creation failed"));
    //int option = 1;
    int check = 0;
    check = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(check));
    if (check == -1)
		throw(std::runtime_error("setting option SO_REUSEADDR failed"));

    check = fcntl(_socket, F_SETFL, O_NONBLOCK);
    if (check == -1)
        throw(std::runtime_error("setting option(O_NONBLOCK) failed"));
    check = bind(_socket, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress)); //bind the socket to the address
    if (check == -1)
        throw(std::runtime_error("binding socket failed"));
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(_socket, (struct sockaddr*)&addr, &addr_len);
    char *serverIP = inet_ntoa(addr.sin_addr);
    check = listen(_socket, SOMAXCONN); // listen for  connections && the socket a passive socket
    if (check == -1)
        throw(std::runtime_error("listen() failed"));

    _serverName = serverIP;
    _poll.fd = _socket;
    _poll.events = POLLIN; // for reading
    _poll.revents = 0;
    _pollFds.push_back(_poll);
    std::cout << MAGENTA << "SERVER STARTED :   " << _socket << RESET <<"    waiting for connection " <<std::endl;
}

void Server::breakSignal(int signum)
{
    if ((signum == SIGINT) || (signum == SIGQUIT))
    {
        std::cout << std::endl << "Signal Received !!!" << std::endl;
        _break = 1;
    }
}


bool Server::accept_cl()
{
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_socket, (struct sockaddr*)&clientAddr, &clientLen);
    // or continue check later
    if (clientFd < 0) return false ;

    fcntl(clientFd, F_SETFL, O_NONBLOCK);
    struct pollfd newPoll;
    newPoll.fd = clientFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    _pollFds.push_back(newPoll);
    _clients[clientFd] = Client(clientFd);
    // here bot initialistaion 
    // logic 
    _clients[clientFd].setIpAddress(inet_ntoa(clientAddr.sin_addr));
    std::cout << "New client connected: " << clientFd << std::endl;
    return true ;
}


void Server::receive(size_t & i)
{
    char buffer[1024];
    int bytes = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);

    if (bytes <= 0) {
        std::cout << "Client disconnected: " << _pollFds[i].fd << std::endl;
        // Client disconnected
        close(_pollFds[i].fd);
        _clients.erase(_pollFds[i].fd);
        _pollFds.erase(_pollFds.begin() + i);
        i--;
    } else {
        buffer[bytes] = '\0';
        std::cout << "BUFFER : " << buffer << std::endl;
        std::string buff(buffer);
        handleBuffer(_pollFds[i].fd, buff);
    }
}

void Server::setup() {

    signal(SIGINT, Server::breakSignal);
    signal(SIGQUIT, Server::breakSignal);
    signal(SIGPIPE, SIG_IGN);
    this->serverSocket();
    while (!_break) {
        int poll_result = poll(&_pollFds[0], _pollFds.size(), -1);
        if (poll_result == -1) {
            if (errno != EINTR) {
                throw std::runtime_error("poll() failed");
            }
            continue;
        }

        for (size_t i = 0; i < _pollFds.size(); i++) {
            if (_pollFds[i].revents & POLLIN) {
                if (_pollFds[i].fd == _socket) {
                    // Accept new client
                    if(!accept_cl())
                        continue ;
                } else {
                    // Receive data from existing client
                    receive(i);
                }
            }
        }
    }
    // struct pollfd bot_poll;
    // bot_poll.fd = bot.getFd(); // Use the bot's file descriptor
    // bot_poll.events = POLLIN;  // Monitor for incoming data
    // bot_poll.revents = 0;
    // _pollFds.push_back(bot_poll);

}

// parse data

void Server::handleBuffer(int fd, std::string &buffer) {

    Client *client = getClient(fd);
    std::vector<std::string> commands;
    client->setBuffer(buffer);
    if(client->getBuffer().find_first_of("\r\n") == std::string::npos)
        return;
    commands = parseData(client);
    for (size_t i = 0; i < commands.size(); i++)
        parseCommand(fd, commands[i]);
    client->clearBuffer();
    return;
}
void Server::botResponse(int fd, std::string &input, Client &client, std::vector<std::string > tokens) {
    (void)client;
    (void)tokens;
    (void)input;

    std::string response = MENU_MESSAGE;
    if (tokens.size() !=  4)
        return(sendResponse(fd, response));
    std::cout << tokens.size() <<std::endl;
    bool tokenGame = ((tokens[2] == "game") && (tokens[3]== "start" || tokens[3]== "a"||  tokens[3]== "b" || tokens[3]== "c" || tokens[3]== "d"));
    bool tokenLog = ((tokens[2] == "logtime") && !tokens[3].empty());

    if ( !tokenGame && !tokenLog)
        return(sendResponse(fd, response));
    if (tokenGame && client.getLevel() == 1 && tokens[3] != "start" &&  client.getStarted() == false)
        return(sendResponse(fd, STARTING_ERROR));
    // start the game ;
    if (tokenGame && client.getLevel() == 1 && client.getStarted() == false)
    {
        sendResponse(fd, WELCOME_MESSAGE);
        sleep (1);
        client.setStarted(true);
    }
    
    QuestionGame game;
    if (tokenGame && tokens[3] == "start")
    {
        QuestionGame::Question q = game.getRandomQuestion(client.getLevel());
        sendResponse(fd, q.problem);
        client.answer = q.correct ;
        client.questionSent = true ;
    }
    else if (tokenGame)
    {
        if (!client.questionSent)
        {
        sendResponse(fd, "You need to start the game first!\n");
        return; // Stop execution here
        }
        if (client.questionSent &&  tokens[3].size() == 1 &&  tokens[3].at(0) == client.answer)
        {
            if (client.getLevel() == 1)
                sendResponse(fd, LVL1_MESSAGE);
            if (client.getLevel() == 2)
                sendResponse(fd, LVL2_MESSAGE);
            if (client.getLevel() == 3)
                sendResponse(fd, LVL3_MESSAGE);
            if (client.getLevel() == 4)
                sendResponse(fd, LVL4_MESSAGE);
            client.addLevel();
            client.questionSent = false ;
        }
        else 
        {
            sendResponse(fd, END_MESSAGE);
            client.setLevel(1);
            client.setStarted(false);
            client.questionSent = false ;
        }
    }
    if (tokenLog)
    {
        Client *find;
        find = this->getClientNick(tokens[3]);
        if (find)
        {
            double time = game.logtime(find->start_time);
            double minutes = floor (time / 60);
            double seconds = fmod(time, 60.0) ;
            std::stringstream ss;
            ss << LOGTIME <<  minutes << "   minute(s) " << seconds << "   seconds " << std::endl;
            sendResponse(fd, ss.str());
        }
        else 
            sendResponse(fd, NOT_FOUND);
    }
    //continue the game ;
}
