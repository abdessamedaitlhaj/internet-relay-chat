#include "../include/Server.hpp"

Server::~Server() {
    close(_socket);
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        close(_pollFds[i].fd);
    }
    for (size_t i = 0; i < _channels.size(); ++i) {
        delete _channels[i];
    }
}

std::string Server::parse_password(std::string password)
{
    if (password.empty())
        throw std::invalid_argument("Error: Password cannot be empty.");
	if (!password.empty()  && std::isspace(password.at(0)) )
		throw std::invalid_argument("Error: Password must not start with a whitespace character.");
    for (size_t i = 0; i < password.length(); ++i) {
        if (std::isspace(password[i]) || !std::isprint(password[i])) {
            throw std::invalid_argument("Error: Password contains invalid characters.");
        }
    }
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
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_port = htons(_port); // network byte order
    _serverAddress.sin_addr.s_addr = INADDR_ANY; // any local machine
    _socket = socket(AF_INET, SOCK_STREAM, 0); // server socket fd
    if(_socket == -1)
		throw(std::runtime_error("socket creation failed"));
    //int option = 1;
    int check;
    int reuse = 1;
    check = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
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
        std::cout << std::endl << YELLOW << "Signal Received !!!" << RESET << std::endl;
        _break = 1;
    }
}
void Server::removeFd(int fd)
{
    for (size_t i = 0; i < _pollFds.size(); i++)
    {
        if (_pollFds[i].fd == fd)
        {
            _pollFds.erase(_pollFds.begin() + i);
            break;
        }
    }
}

void Server::removeClient(int fd)
{
    std::map<int, Client>::iterator it = _clients.find(fd);
    if (it != _clients.end()) {
        _clients.erase(it);
    }
}

void Server::removeChannel(Client &client)
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->getclient(client.getNickName()))
        {
            if (_channels[i]->isOperator(&client))
                _channels[i]->removeOperator(&client);
            _channels[i]->removeMember(&client);
            if (_channels[i]->getclientsnumber() != 0)
            {
                std::string response = ":" + client.getHostName() + client.getIpAddress() + " QUIT " + ":" + "Quit" + CRLF;
                _channels[i]->broadcastToAll(response);
            }
            else
            {
                _channels.erase(_channels.begin() + i);
                delete _channels[i];
                i--;
            }
        }
    }
}

bool Server::accept_cl()
{
    int check;
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    int clientFd = accept(_socket, (struct sockaddr*)&clientAddr, &clientLen);
    // or continue check later
    if (clientFd < 0) return false ;

    check = fcntl(clientFd, F_SETFL, O_NONBLOCK);
    if (check == -1)
        throw(std::runtime_error("setting option(O_NONBLOCK) failed"));
    struct pollfd newPoll;
    newPoll.fd = clientFd;
    newPoll.events = POLLIN;
    newPoll.revents = 0;
    _pollFds.push_back(newPoll);
    _clients[clientFd] = Client(clientFd);
    _clients[clientFd].setIpAddress(inet_ntoa(clientAddr.sin_addr));
    std::cout << GREEN << "Client <" << clientFd << "> Connected: " << RESET << std::endl;
    return true ;
}

void Server::receive(size_t & i)
{
    char buffer[2048];
    int bytes = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);

    if (sizeof(buffer) == 0)
        return;

    if (bytes <= 0) {
        std::cout << RED << "Client <" << _pollFds[i].fd << "> Disconnected" << RESET << std::endl;
        removeChannel(_clients[_pollFds[i].fd]);
        removeClient(_pollFds[i].fd);
        close(_pollFds[i].fd);
        _pollFds.erase(_pollFds.begin() + i);
        i--;
    } else {
        buffer[bytes] = '\0';
        std::cout << GREEN << "BUFFER : " << RESET << buffer <<  std::endl;
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
        if (poll_result == -1 && !_break) {
        {
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
}

void Server::handleBuffer(int fd, std::string &buffer) {

    Client *client = getClient(fd);
    std::vector<std::string> commands;
    client->setBuffer(buffer);
    if(client->getBuffer().find_first_of("\r\n") == std::string::npos)
        return;
    commands = parseData(client);
    for (std::vector<std::string>::iterator it = commands.begin(); it != commands.end(); ++it)
        parseCommand(fd, *it);
    client->clearBuffer();
    return;
}
