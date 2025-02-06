#include "../../include/Server.hpp"

void    Server::handlePrivmsg(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));

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
        std::string last = tokens[tokens.size() - 1];
        size_t pos = last.find(":");
        std::string trailing = pos != std::string::npos ? last.substr(pos + 1) : last;
        if (tokens[1][0] == '#') {
            Channel *channel = getChannel(target);
            if (!channel->isMember(&client)) {
                sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), target));
                return;
            }
            std::string response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG " + target + " :" + trailing + CRLF;
            channel->broadcast(response, &client);
        }
        else {
            std::string response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG " + target + " :" + trailing + CRLF;
            Client *cli = getClientNick(target);
            sendResponse(cli->getFd(), response);
        }
    }
}