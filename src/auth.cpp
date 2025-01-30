#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/numericReplies.hpp"


// pass
void Server::handlePass(int fd, std::vector<std::string> &tokens, Client &client) {

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(tokens[0]));
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

void Server::handleNick(int fd, std::vector<std::string> &tokens, Client &client) {

    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string(tokens[0])));
        return;
    }
    if (!nickNameValid(tokens[1])) {
        sendResponse(fd, ERR_ERRONEUSNICKNAME(tokens[1]));
        return;
    }
    if (isNicknameInUse(tokens[1])) {
        sendResponse(fd, ERR_NICKNAMEINUSE(tokens[1]));
        return;
    }
    client.setNickName(tokens[1]);
}

// username
void Server::handleUser(int fd,std::vector<std::string> &tokens, std::string &trailing, Client &client) {

    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string(std::string("*"))));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(std::string(tokens[0])));
        return;
    }
    client.setUserName(tokens[1]);
    client.setRealName(trailing);
}