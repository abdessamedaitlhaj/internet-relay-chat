#include "../include/Server.hpp"

void Server::handlePass(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string("*"), tokens[0]));
        return;
    }
    if (!client.getPassword().empty()) {
        sendResponse(fd, ERR_ALREADYREGISTERED(std::string("*")));
        return;
    }
    if (client.isRegistered()) {
        sendResponse(fd, ERR_ALREADYREGISTERED(std::string("*")));
        return;
    }
    if (tokens[1] != _password || tokens.size() > 2) {
        sendResponse(fd, ERR_PASSMISMATCH(std::string("*")));
        return;
    }
    client.setPassword(tokens[1]);
}

bool Server::isNickNameInUse(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickName() == nickname) {
            return true;
        }
    }
    return false;
}

bool isNickNameValid(std::string &name) {

    if (isdigit(name[0]) || name.empty() || name[0] == '#' || name[0] == '&' || name[0] == ':')
        return false;
    for (size_t i = 1; i < name.length(); ++i)
        if (!std::isalnum(name[i]) && name[i] != '{' && name[i] != '}' && name[i] != '[' && name[i] != ']' && name[i] != '\\' && name[i] != '|')
            return false;
    return true;
}

void Server::handleNick(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));
    if (client.getPassword().empty()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NONICKNAMEGIVEN(std::string("*")));
        return;
    }
    if (!isNickNameValid(tokens[1]) || (tokens[1] == "bot")) {
        sendResponse(fd, ERR_ERRONEUSNICKNAME(tokens[1]));
        return;
    }

    if (isNickNameInUse(tokens[1])) {
        sendResponse(fd, ERR_NICKNAMEINUSE(tokens[1]));
        return;
    }
    if (client.isRegistered()) {
        std::string response = ":" + client.getHostName() + client.getIpAddress() + " NICK " + tokens[1] + CRLF;
        client.setNickName(tokens[1]);
        sendResponse(fd, response);
        return;
    }
    if (!client.getNickName().empty()) {
        sendResponse(fd, ERR_ALREADYREGISTERED(client.getNickName()));
        return;
    }
    client.setNickName(tokens[1]);

    if (!client.getPassword().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
        client.setRegistered(true);
        sendResponse(fd, RPL_WELCOME(client.getNickName(), client.getUserName(), client.getHostName(), client.getIpAddress()));
    }
}

void Server::handleUser(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));

    if (client.getPassword().empty()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() < 5 || tokens[4].empty()) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }
    if (!client.getUserName().empty()) {
        sendResponse(fd, ERR_ALREADYREGISTERED(client.getNickName()));
        return;
    }
    std::string last;
    if (tokens.size() > 5) {
        last = getMsg(tokens, 4);
    } else if (tokens.size() == 5) {
        last = tokens[4];
    }
    size_t pos = last.find_first_of(":");
    std::string realName = pos != std::string::npos ? last.substr(pos + 1) : last;
    if (realName.empty()) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }
    client.setUserName(tokens[1]);
    client.setRealName(realName);
    if (!client.getPassword().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
        client.setRegistered(true);
        sendResponse(fd, RPL_WELCOME(client.getNickName(), client.getUserName(), client.getHostName(), client.getIpAddress()));
    }
}