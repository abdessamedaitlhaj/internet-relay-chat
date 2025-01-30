#include "../include/Server.hpp"
#include "../include/numericReplies.hpp"

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
    std::cout << "pass  :" << _password << std::endl;
    std::cout << "port  :" << _port << std::endl;
}

void Server::serverSocket() 
{
    //add comment later
    serverAddress.sin_family = AF_INET; //ipv4
    serverAddress.sin_port = htons(_port); // network byte order
    serverAddress.sin_addr.s_addr = INADDR_ANY; // any local machine
    _socket = socket(AF_INET, SOCK_STREAM, 0); // server socket fd
    if(_socket == -1)
		throw(std::runtime_error("socket creation failed"));
    int option = 1;
    int check = 0;
    check = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &check, sizeof(check));
    if (check == -1)
		throw(std::runtime_error("setting option SO_REUSEADDR failed"));

    check = fcntl(_socket, F_SETFL, O_NONBLOCK);
    if (check == -1)
        throw(std::runtime_error("setting option(O_NONBLOCK) failed"));
    check = bind(_socket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)); //bind the socket to the address
    if (check == -1) 
        throw(std::runtime_error("binding socket failed"));
    check = listen(_socket, SOMAXCONN); // listen for  connections && the socket a passive socket
    if (check == -1) 
        throw(std::runtime_error("listen() failed"));
    
    _poll.fd = _socket;
    _poll.events = POLLIN; // for reading
    _poll.revents = 0;
    _pollfds.push_back(_poll);
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

void Server::setup() {

    signal(SIGINT, Server::breakSignal);
    signal(SIGQUIT, Server::breakSignal);
    signal(SIGPIPE, SIG_IGN);
    this->serverSocket();

    while (!_break) {
        int poll_result = poll(&_pollfds[0], _pollfds.size(), -1);
        if (poll_result == -1) {
            if (errno != EINTR) {
                throw std::runtime_error("poll() failed");
            }
            continue;
        }

        for (size_t i = 0; i < _pollfds.size(); i++) {
            if (_pollfds[i].revents & POLLIN) {
                if (_pollfds[i].fd == _socket) {
                    // Accept new client
                    struct sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
                    int clientFd = accept(_socket, (struct sockaddr*)&clientAddr, &clientLen);

                    if (clientFd < 0) continue;

                    fcntl(clientFd, F_SETFL, O_NONBLOCK);
                    struct pollfd newPoll;
                    newPoll.fd = clientFd;
                    newPoll.events = POLLIN;
                    newPoll.revents = 0;
                    _pollfds.push_back(newPoll);
                    _clients[clientFd] = Client(clientFd);
                } else {
                    // Receive data from existing client
                    char buffer[1024];
                    int bytes = recv(_pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);

                    if (bytes <= 0) {
                        // Client disconnected
                        close(_pollfds[i].fd);
                        _clients.erase(_pollfds[i].fd);
                        _pollfds.erase(_pollfds.begin() + i);
                        i--;
                    } else {
                        buffer[bytes] = '\0';
                        std::cout << "BUFFER : " << buffer << std::endl;
                        std::string buff(buffer);
                        handleBuffer(_pollfds[i].fd, buff);
                    }
                }
            }
        }
    }
}

// parse data

void Server::handleBuffer(int fd, std::string &buffer) {

    Client *client = getClient(fd);
    std::vector<std::string> commands;
    client->setBuffer(buffer);
    if(client->getBuffer().find_first_of("\r\n") == std::string::npos)
        return;
    commands = parseData(client);
    for (int i = 0; i < commands.size(); i++)
        parseCommand(fd, commands[i]);
    client->clearBuffer();
    return;
}

std::vector<std::string> Server::parseData(Client* client) {

    std::vector<std::string> lines;
    std::string buffer = client->getBuffer();
    std::string line;
    std::istringstream stream(buffer);

    while (std::getline(stream, line)) {

        if (line.empty())
            continue;
        size_t pos = line.find_first_of("\r\n");
        if (pos != std::string::npos) {
            line.erase(pos);
        }
        lines.push_back(line);
    }
    return lines;
}

void sendResponse(int fd, const std::string& response) {
    if (send(fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client " << fd << std::endl;
    }
}

Client* Server::getClient(int fd) {
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        return &(it->second);
    }
    return NULL;
}

void Server::sendResponse(int fd, const std::string& response) {
    if (send(fd, response.c_str(), response.length(), 0) == -1) {
        std::cerr << "Error: Failed to send response to client " << fd << std::endl;
    }
}

std::vector<std::string> Server::split(const std::string& str, const std::string& delimiters) {

    std::vector<std::string> tokens;
    size_t start = str.find_first_not_of(delimiters);
    size_t end = 0;

    while ((end = str.find_first_of(delimiters, start)) != std::string::npos) {
        if (end > start) {
            tokens.push_back(str.substr(start, end - start));
        }
        start = str.find_first_not_of(delimiters, end);
    }

    if (start != std::string::npos) {
        tokens.push_back(str.substr(start));
    }

    return tokens;
}

void Server::parseCommand(int fd, std::string input) {

    Client* client = getClient(fd);
    std::string command, params, trailing;
    std::vector<std::string> tokens;
    size_t pos = 0;

    if (!client || input.empty())
        return;
    pos = input.find(":");
    if (pos != std::string::npos) {
        tokens = Server::split(input.substr(0, pos), std::string("\t "));
        command = tokens[0];
        trailing = input.substr(pos);
    } else {
        tokens = Server::split(input, std::string("\t "));
        command = tokens[0];
    }

    std::cout << "commad : " << command << std::endl;

    for (size_t i = 0; i < command.length(); ++i) {
        command[i] = toupper(command[i]);
    }

    if (command == "PASS")
        handlePass(fd, tokens, *client);
    else if (command == "NICK")
        handleNick(fd, tokens, *client);
    else if (command == "USER")
        handleUser(fd, tokens, trailing, *client);
    else if (command == "TOPIC")
        handleTopic(fd, tokens, trailing, *client);
    else
        sendResponse(fd, ERR_NOTREGISTERED(std::string("*")));

    if (!client->getPassword().empty() && !client->getNickname().empty() && !client->getUsername().empty()) {
        client->setRegistered(true);
        sendResponse(fd, "001 " + client->getNickname() + " :Welcome to the Internet Relay Network " + client->getNickname() + "!" + client->getUsername() + "@hostname\r\n");
    }
}

Channel *Server::getChannel(std::string &name) {

    for (int i = 0; i < _channels.size(); i++)
        if (_channels[i].getName() == name)
            return (&_channels[i]);
    return (NULL);
}

