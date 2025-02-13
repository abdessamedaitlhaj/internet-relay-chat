#include "../../include/Server.hpp"

std::string Server::getMsg(std::vector<std::string> &tokens, int start) {
    std::string msg;
    for (size_t i = start; i < tokens.size(); ++i) {
        i == tokens.size() - 1 ? msg += tokens[i] : msg += tokens[i] + " ";
    }
    return msg;
}

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

    std::vector<std::string> targts = split(tokens[1], std::string(","));
    std::string target;
    std::string trailing;
    std::string response;
    for (size_t i = 0; i < targts.size(); ++i) {
        std::string last;
        size_t pos;
        if (tokens.size() > 3)
            last = tokens[2].find_first_of(":") != std::string::npos ? getMsg(tokens, 2) : tokens[2];
        else if (tokens.size() == 3)
            last = tokens[2];

        pos = last.find_first_of(":");
        if (pos != std::string::npos)
            trailing = last.substr(pos + 1);
        else
            trailing = last;
        
        target = targts[i];
        if (target == "Bot")
        {
            std::string res = ":BotNick PRIVMSG " + client.getNickName() + " :Responding to " + client.getNickName() + "\r\n";
            sendResponse(fd, res);
            botResponse(fd, input , client , tokens);
        }
        else if (target[0] == '#') {
            std::string channelName = target.substr(1);
            if (!getChannel(channelName)) {
                sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), "#" + channelName));
                continue ;
            }
            Channel *channel = getChannel(channelName);
            if (!channel->isMember(&client)) {
                sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
                continue ;
            }
            response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG #" + channelName + " :" + trailing + CRLF;
            channel->broadcast(response, &client);
        }
        else {
            Client *cli = getClientNick(target);
            if (!cli) {
                sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), target));
                continue ;
            }
            response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG " + target + " :" + trailing + CRLF;
            sendResponse(cli->getFd(), response);
        }
    }
}