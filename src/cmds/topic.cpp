#include "../../include/Server.hpp"

bool    Server::channelNameValid(std::string &channelName) {

    if (channelName.empty() || channelName.length() < 2 || (channelName[0] != '#' && channelName[0] != '&' \
        && channelName[0] != '!' && channelName[0] != '+') || channelName.length() > 50) {
        return false;
    }
    for (size_t i = 1; i < channelName.length(); ++i) {

        if (!std::isalnum(channelName[i])) {
            return false;
        }
    }
    return true;
}

void Server::topic(std::vector<std::string> tokens, std::string &trailing, Client &client) {

    int fd = client.getFd();
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(tokens[0]));
        return;
    }

    if (!channelNameValid(tokens[1])) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(std::string("#" + tokens[1].substr(1))));
        return;
    }
    std::string channelName = tokens[1].substr(1);
    addChannel(new Channel(std::string("d")));
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(std::string("#" + channelName)));
        return;
    }
    channel->addMember(&client);
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(std::string("#" + channelName)));
        return;
    }
    if (tokens.size() == 2) {
        if (channel->getTopic().empty()) {
            sendResponse(fd, RPL_NOTOPIC(std::string("#" + channelName)));
        } else {
            sendResponse(fd, RPL_TOPIC(std::string("#" + channelName), channel->getTopic()));
        }
        return;
    }

    // Check topic restrictions
    // if (channel->isTopicRestricted() && !channel->isOperator(client.getNickname())) {
    //     sendResponse(fd, ERR_CHANOPRIVSNEEDED(channelName));
    //     return;
    // }

    channel->setTopic(trailing);
    sendResponse(fd, RPL_TOPIC(std::string("#" + channelName), trailing));
    //channel->broadcast(":" + client.getNickname() + " TOPIC " + channelName + " :" + trailing);
}