#include "../../include/Server.hpp"
#include "../../include/numericReplies.hpp"
#include "../../include/Channel.hpp"


bool    Server::channelNameValid(std::string &channelName) {

    if (channelName.empty() || channelName[0] != '#') {
        return false;
    }
    for (size_t i = 1; i < channelName.length(); ++i) {
        if (!std::isalnum(channelName[i]) && channelName[i] != '-' && channelName[i] != '_') {
            return false;
        }
    }
    return true;
}

void Server::handleTopic(int fd, std::vector<std::string> tokens, std::string &trailing, Client &client) {
    // 1. Basic validation
    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(tokens[0]));
        return;
    }
    std::string channelName = tokens[1];
    // if (!channelNameValid(channelName)) {
    //     sendResponse(fd, ERR_NOSUCHCHANNEL(channelName));
    //     return;
    // }
    addChannel(new Channel(std::string("d")));
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(channelName));
        return;
    }
    std::cout << "channelName : " << channelName << std::endl;
    channel->addMember(&client);
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(channelName));
        return;
    }
    if (tokens.size() == 2) {
        if (channel->getTopic().empty()) {
            sendResponse(fd, RPL_NOTOPIC(channelName));
        } else {
            sendResponse(fd, RPL_TOPIC(channelName, channel->getTopic()));
        }
        return;
    }

    // 6. Setting new topic
    // Check topic restrictions
    // if (channel->isTopicRestricted() && !channel->isOperator(client.getNickname())) {
    //     sendResponse(fd, ERR_CHANOPRIVSNEEDED(channelName));
    //     return;
    // }

    // Set new topic and broadcast
    channel->setTopic(trailing);
    sendResponse(fd, RPL_TOPIC(channelName, trailing));
    //channel->broadcast(":" + client.getNickname() + " TOPIC " + channelName + " :" + trailing);
}