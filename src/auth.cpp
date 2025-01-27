#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/numericReplies.hpp"


// pass
void Server::handlePass(int fd, std::string &params, std::string &command, Client &client) {

    if (params.empty()) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(command));
        return;
    }
    if (client.isRegistered()) {
        sendResponse(fd, ERR_ALREADYREGISTERED(std::string(" ")));
        return;
    }
    if (params != _password) {
        sendResponse(fd, ERR_PASSMISMATCH(std::string("*")));
        return;
    }
    client.setPassword(params);
    client.setRegistered(true);
}

// nickname
bool Server::isNicknameInUse(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname) {
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

void Server::handleNick(int fd, std::string &params, std::string &command, Client &client) {

    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(params)));
        return;
    }
    if (params.empty()) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string(command)));
        return;
    }
    if (!nickNameValid(params)) {
        sendResponse(fd, ERR_ERRONEUSNICKNAME(params));
        return;
    }
    if (isNicknameInUse(params)) {
        sendResponse(fd, ERR_NICKNAMEINUSE(params));
        return;
    }
    client.setNickName(params);
}

// username
void Server::handleUser(int fd, std::string &params, std::string &trailing, std::string &command, Client &client) {

    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(params)));
        return;
    }
    if (params.empty()) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string(command)));
        return;
    }
    client.setUserName(params);
    client.setRealName(trailing);
}