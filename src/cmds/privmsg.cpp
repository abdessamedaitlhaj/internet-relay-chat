#include "../../include/Server.hpp"


bool Server::isChannel(int fd, std::string &target, Client &client, std::string &trailing) {

    if (target[0] == '#') {
        std::string channelName = target.substr(1);
        if (!getChannel(channelName)) {
            sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), "#" + channelName));
            return true;
        }
        Channel *channel = getChannel(channelName);
        if (!channel->isMember(&client)) {
            sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
            return true;
        }
        std::string response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG #" + channelName + " :" + trailing + CRLF;
        channel->broadcast(response, &client);
        return true;
    }
    return false;
}

bool Server::isClient(int fd, std::string &target, Client &client, std::string &trailing) {

    Client *cli = getClientNick(target);
    if (!cli) {
        sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), target));
        return true;
    }
    std::string response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG " + target + " :" + trailing + CRLF;
    sendResponse(cli->getFd(), response);
    return true;
}

void Server::handlePrivmsg(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NORECIPIENT(client.getNickName()));
        return;
    } else if (tokens.size() == 2) {
        sendResponse(fd, ERR_NOTEXTTOSEND(client.getNickName()));
        return;
    }
    std::vector<std::string> targts = split(tokens[1], std::string(","));
    std::string target;
    std::string trailing;
    std::string response;
    for (size_t i = 0; i < targts.size(); ++i) {
        trailing = getTrailing(tokens, trailing, input);
        target = targts[i];
        if (isChannel(fd, target, client, trailing))
            continue;
        else if (isClient(fd, target, client, trailing))
            continue;
    }
}