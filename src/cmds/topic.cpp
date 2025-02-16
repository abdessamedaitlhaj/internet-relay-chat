#include "../../include/Server.hpp"

bool    Server::channelNameValid(std::string &channelName) {

    if (channelName.empty() || channelName.length() < 2 || channelName[0] != '#' || channelName.length() > 50)
        return false;
    for (size_t i = 1; i < channelName.length(); ++i) {
        if (!std::isalnum(channelName[i])) {
            return false;
        }
    }
    return true;
}

void Server::handleTopic(int fd, std::string &input, Client &client) {

    std::vector<std::string> tokens;
    std::string trailing;

    tokens = split(input, std::string("\t "));
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }

    if (!channelNameValid(tokens[1])) {
        std::string name = tokens[1][0] != '#' ? tokens[1] : tokens[1].substr(1);
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), name));
        return;
    }
    std::string channelName = tokens[1].substr(1);
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channelName));
        return;
    }
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
        return;
    }
    if (tokens.size() == 2) {
        if (channel->getTopic().empty()) {
            sendResponse(fd, RPL_NOTOPIC(client.getNickName(), client.getHostName(), channelName));
        } else {
            sendResponse(fd, RPL_TOPIC(client.getNickName(), client.getHostName(), client.getIpAddress(), channelName, channel->getTopic()));
            sendResponse(client.getFd(), RPL_TOPICWHOTIME(client.getNickName(), channelName, client.getNickName(), std::to_string(channel->getTopicTime())));
        }
        return;
    }
    if (channel->getTopicRestriction() && !channel->isOperator(&client)) {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    trailing = getTrailing(tokens, trailing, input);
    channel->setTopic(trailing);
    std::string response = ":" + client.getHostName() + client.getIpAddress() + " TOPIC #" + channelName + " :" + trailing + CRLF;
    channel->broadcastToAll(response);
}