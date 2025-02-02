#include "../../include/Server.hpp"

void    Server::privmsg(std::vector<std::string> &tokens, std::string &trailing, Client &client) {

    int fd = client.getFd();
    if (tokens.size() < 3) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(tokens[0]));
        return;
    }

    std::string target = tokens[1][0] == '#' ? tokens[1].substr(1) : tokens[1];
    if (tokens[1][0] == '#' && !getChannel(target)) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(tokens[1]));
        return;
    }
    else if (!getClientNick(target)) {
        sendResponse(fd, ERR_NOSUCHNICK(target));
        return;
    }
    else {
        if (tokens[1][0] == '#') {
            Channel *channel = getChannel(target);
            if (!channel->isMember(&client)) {
                sendResponse(fd, ERR_NOTONCHANNEL(tokens[1]));
                return;
            }
            channel->broadcast(":" + client.getNickname() + " PRIVMSG " + tokens[1] + " :" + trailing);
        }
        else {
            Client *cli = getClientNick(target);
            sendResponse(cli->getFd(), ":" + cli->getNickname() + " PRIVMSG " + target + " :" + trailing);
        }
    }
}