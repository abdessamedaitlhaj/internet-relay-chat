#include "../..//include/Server.hpp"

void Server::handleMode(int fd, std::string &input, Client &client) {


    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }
    std::string channelName = tokens[1].substr(1);
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channelName));
        return;
    }
    if (tokens.size() == 2 && channel) {
        sendResponse(fd, RPL_CHANNELMODEIS(client.getNickName(), client.getNickName(), std::string("n yet")));
        sendResponse(fd, RPL_CREATIONTIME(client.getNickName(), client.getNickName(), std::string("n yet")));
    }
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), tokens[1].substr(1)));
        return;
    }
    if (tokens.size() == 3) {
        std::string last = tokens[tokens.size() - 1];
        size_t pos = last.find(":");
        std::string trailing = pos != std::string::npos ? last.substr(pos + 1) : last;
        if (tokens[2] == "+o") {
            channel->addOperator(&client);
            sendResponse(fd, RPL_UMODEIS(client.getNickName(), "+o"));
        } else if (trailing == "-o") {
            channel->removeOperator(&client);
            sendResponse(fd, RPL_UMODEIS(client.getNickName(), "-o"));
        }
        }
    }
    else {
        sendResponse(fd, ERR_UMODEUNKNOWNFLAG(client.getNickName()));
    }
}