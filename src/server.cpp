#include "../include/server.hpp"

std::string server::parse_password(std::string password)
{
	if (!password.empty()  && std::isspace(password.at(0)) )
		throw std::invalid_argument("Error: Password must not start with a whitespace character.");
	return(password);
}

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