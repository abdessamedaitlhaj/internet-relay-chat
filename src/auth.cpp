#include "../include/Server.hpp"

// pass
void Server::handlePass(int fd, std::vector<std::string> &tokens, Client &client) {

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string("*"), tokens[0]));
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

// nickname
bool Server::isNickNameInUse(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickName() == nickname) {
            return true;
        }
    }
    return false;
}

bool nickNameValid(std::string &name) {

    for (size_t i = 1; i < name.length(); ++i)
        if (!std::isalnum(name[i]))
            return false;
    return true;
}

void Server::handleNick(int fd, std::vector<std::string> &tokens, Client &client) {

    if (client.getPassword().empty()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string("*"), tokens[0]));
        return;
    }
    if (!nickNameValid(tokens[1])) {
        sendResponse(fd, ERR_ERRONEUSNICKNAME(tokens[1]));
        return;
    }
    if (isNickNameInUse(tokens[1])) {
        sendResponse(fd, ERR_NICKNAMEINUSE(tokens[1]));
        return;
    }
    client.setNickName(tokens[1]);
    
    if (!client.getPassword().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
        client.setRegistered(true);
        sendResponse(fd, RPL_WELCOME(client.getNickName(), client.getUserName(), client.getHostName(), client.getIpAddress()));
    }
}

// username
void Server::handleUser(int fd, std::vector<std::string> &tokens, std::string &trailing, Client &client) {

    if (client.getPassword().empty()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }
    client.setUserName(tokens[1]);
    client.setRealName(trailing);
    if (!client.getPassword().empty() && !client.getNickName().empty() && !client.getUserName().empty()) {
        client.setRegistered(true);
        sendResponse(fd, RPL_WELCOME(client.getNickName(), client.getUserName(), client.getHostName(), client.getIpAddress()));
    }
}