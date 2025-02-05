#include "../include/Server.hpp"

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
        if (tokens.size() > 2)
            trailing = tokens[2];
    }

    for (size_t i = 0; i < command.length(); ++i) {
        command[i] = toupper(command[i]);
    }


    if (command == "PASS")
        handlePass(fd, tokens, *client);
    else if (command == "NICK")
        handleNick(fd, tokens, *client);
    else if (command == "USER")
        handleUser(fd, tokens, trailing, *client);
    else if (client->isRegistered()) {
        if (command == "TOPIC")
            handleTopic(fd, tokens, trailing, *client);
        else if (command == "PRIVMSG")
            handlePrivmsg(fd, tokens, trailing, *client);
        else if (command == "JOIN")
            handleJoin(fd, tokens, trailing, *client);
        else
            sendResponse(fd, ERR_UNKNOWNCOMMAND(client->getNickName(), command));
    } else
        sendResponse(fd, ERR_NOTREGISTERED(client->getNickName()));

}

Channel *Server::getChannel(std::string &name) {

    for (size_t i = 0; i < _channels.size(); i++)
        if (_channels[i]->getName() == name)
            return (_channels[i]);
    return (NULL);
}

void Server::addChannel(Channel *channel) {
    _channels.push_back(channel);
}

Client *Server::getClientNick(std::string &nick) {

    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickName() == nick)
            return &(it->second);
    }
    return (NULL);
}
