#include "../../include/Server.hpp"

void    Server::handlePrivmsg(int fd, std::vector<std::string> &tokens, std::string &trailing, Client &client) {

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NORECIPIENT(client.getNickName()));
        return;
    } else if (tokens.size() == 2) {
        sendResponse(fd, ERR_NOTEXTTOSEND(client.getNickName()));
        return;
    }

    std::string target = tokens[1][0] == '#' ? tokens[1].substr(1) : tokens[1];
    if ((tokens[1][0] == '#' && !getChannel(target)) || !getClientNick(target)) {
        sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), tokens[1]));
        return;
    }
    else {
        if (tokens[1][0] == '#') {
            Channel *channel = getChannel(target);
            if (!channel->isMember(&client)) {
                sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), target));
                return;
            }
            std::string response = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + target + " :" + trailing + CRLF;
            channel->broadcast(response);
        }
        else {
            std::string response = ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + target + " :" + trailing + CRLF;
            Client *cli = getClientNick(target);
            sendResponse(cli->getFd(), ":" + client.getNickName() + "!" + client.getUserName() + "@" + client.getHostName() + " PRIVMSG " + target + " :" + trailing);
        }
    }
}