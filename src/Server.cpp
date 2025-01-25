#include "../include/Server.hpp"

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

Client* Server::getClient(int fd) {
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        return &(it->second);
    }
    return NULL;
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
                        Client& client = _clients[_pollfds[i].fd];
                        client.setBuffer(std::string(buffer));

                        std::vector<std::string> tokens = parseData(&client);
                        for (size_t j = 0; j < tokens.size(); j++) {
                            parseCommand(tokens[j], _pollfds[i].fd);
                        }
                    }
                }
            }
        }
    }
}

std::vector<std::string> Server::parseData(Client* client) {
    std::vector<std::string> lines;
    std::string buffer = client->getBuffer();
    std::string line;
    std::istringstream stream(buffer);

    while (std::getline(stream, line)) {
        if (line.empty()) continue;
        size_t pos = line.find_first_of("\r\n");
        if (pos != std::string::npos) {
            line.erase(pos);
        }
        lines.push_back(line);
    }
    return lines;
}

void Server::parseCommand(std::string input, int fd) {

    Client* client = getClient(fd);
    if (!client) return;

    std::string prefix, command, params, trailing;

    if (input.empty()) return;

    size_t pos = 0;
    if (input[0] == ':') {
        pos = input.find(' ');
        if (pos == std::string::npos) return;
        prefix = input.substr(1, pos - 1);
        input = input.substr(pos + 1);
    }

    pos = input.find(' ');
    command = input.substr(0, pos);
    std::cout << command << std::endl;
    if (pos != std::string::npos) {
        params = input.substr(pos + 1);
        pos = params.find(':');
        if (pos != std::string::npos) {
            trailing = params.substr(pos + 1);
            params = params.substr(0, pos);
        }
    }

    if (command == "PASS") {
        // Handle PASS command
    } else if (command == "NICK") {
        // Handle NICK command
    } else if (command == "USER") {
        // Handle USER command
    } else if (command == "JOIN") {
        // Handle JOIN command
    } else if (command == "PART") {
        // Handle PART command
    } else if (command == "PRIVMSG") {
        // Handle PRIVMSG command
    } else if (command == "QUIT") {
        // Handle QUIT command
    } else if (command == "PING") {
        // Handle PING command
    } else if (command == "PONG") {
        // Handle PONG command
    } else {
        // Handle unknown command
    }
}

// void server::retrieveData(int fd) {

//     Client *client = getClient(fd);
//     vector<std::string> tokens;
//     char buffer[1024];
//     int bytes = recv(fd, buffer, 1024, 0);
//     if (bytes <= 0) {
//         // client disconnected
//         // for (int i = 0; i < _clients.size(); i++)
//         //     if (_clients[i].getFd() == fd) {
//         //         _clients.erase(_clients.begin() + i);
//         //         break;
//         //     }
//         // RM clients
//         // RM fds
//         // RM channels

//     } else {
//         buffer[bytes] = '\0';
//         client.setBuffer(buffer);
//         // client->appendToBuffer(data);
//         tokens(parseData(client));
//         for (int i = 0; i < tokens.size(); i++) {
//             // parse and execute command
//             parseCommand(tokens[i], fd);
//         }
//         std::cout << "Received: " << buffer << std::endl;
//     }

// }
