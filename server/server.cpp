#include "../include/server.hpp"

std::string server::parse_password(std::string password)
{
	if (!password.empty()  && std::isspace(password.at(0)) )
		throw std::invalid_argument("Error: Password must not start with a whitespace character.");
	return(password);
}

int server::_break = 0;

int server::parse_port(std::string port)
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

server::server(char **av)
{
	std::string port(av[1]);
	std::string password(av[2]);
	_port = parse_port(port);
	_password = parse_password(password);
	std:: cout << "pass  :"    << _password << std::endl;
	std:: cout << "port  :"  << _port << std::endl;
}

void server::serverSocket() 
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


void server::BreakSignal(int signum)
{
    if ((signum == SIGINT) || (signum == SIGQUIT))
    {
        std::cout << std::endl << "Signal Received !!!" << std::endl;
        _break = 1;
    }
}

void server::setup() {
	//handle signals later check sigpipe case later
    signal(SIGINT, server::BreakSignal);
	signal(SIGQUIT, server::BreakSignal);
	signal(SIGPIPE, SIG_IGN);
	this->serverSocket();
    while (!_break)
    {
        //check if poll failed later 
        poll(&_pollfds[0], _pollfds.size(), -1);
        for(size_t i = 0 ; i < _pollfds.size(); i++)
        {
            bool hasDataToRead = (_pollfds[i].revents & POLLIN) != 0;
            if (hasDataToRead) {
                if (_pollfds[i].fd == _socket)
                {
                    // new client
                    ;
                }
                else
                {
                    // receive data
                    ;
                }
            }
        }
    }
}