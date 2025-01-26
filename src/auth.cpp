#include "../include/Server.hpp"
#include "../include/Client.hpp"



void Server::handlePass(int fd, std::string &params, Client &client) {

    if (client.isRegistered()) {
        sendResponse(fd, "462 :You may not reregister\r\n");
        return;
    }
    if (params != _password) {
        sendResponse(fd, "464 :Password incorrect\r\n");
        return;
    }
    client.setPassword(params);
}

bool Server::isNicknameInUse(const std::string& nickname) {
    for (std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if (it->second.getNickname() == nickname) {
            return true;
        }
    }
    return false;
}

bool nickNameValid(std::string &name) {

    if (name.length() < 1 || name.length() > 9) {
        return false;
    }

    for (size_t i = 1; i < name.length(); ++i)
        if (!std::isalnum(name[i]))
            return false;
    return true;
}

void Server::handleNick(int fd, std::string &params, Client &client) {

    if (!client.isRegistered()) {
        sendResponse(fd, "462 :You have not reregistered\r\n");
        return;
    }
    if (params.empty()) {
        sendResponse(fd, "431 :No nickname given\r\n");
        return;
    }
    if (isNicknameInUse(params)) {
        sendResponse(fd, "433 " + params + " :Nickname is already in use\r\n");
        return;
    }
    if (nickNameValid(params))
        client.setNickName(params);
    else
        sendResponse(fd, ": 432 #### :Erroneus nickname");
}

void Server::handleUser(int fd, std::string &params, std::string &trailing, Client &client) {

    if (client.isRegistered()) {
        sendResponse(fd, "462 :You may not reregister\r\n");
        return;
    }
    if (params.empty()) {
        sendResponse(fd, "461 USER :Not enough parameters\r\n");
        return;
    }
    client.setUserName(params);
    client.setRealName(trailing);
}