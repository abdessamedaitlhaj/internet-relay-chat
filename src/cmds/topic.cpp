#include "../../include/Server.hpp"
#include "../../include/numericReplies.hpp"
#include "../../include/Channel.hpp"


void Server::handleTopic(int fd, std::vector<std::string> tokens, std::string &trailing, Client &client) {

    std::string channelName, topic;

    if (!client.isRegistered()) {
        sendResponse(fd, ERR_NOTREGISTERED(std::string("*")));
        return;
    }
    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(tokens[0]));
        return;
    }
    channelName = tokens[1];
    if ()
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(channelName));
        return;
    }

    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(channelName));
        return;
    }

    // if (topic.empty()) {
    //     if (channel->getTopic().empty()) {
    //         sendResponse(fd, RPL_NOTOPIC(channelName));
    //     } else {
    //         sendResponse(fd, RPL_TOPIC(channelName, channel->getTopic()));
    //     }
    //     return;
    // }

    // if (channel->isTopicRestricted() && !channel->isOperator(client.getNickname())) {
    //     sendResponse(fd, ERR_CHANOPRIVSNEEDED(channelName));
    //     return;
    // }

    // channel->setTopic(topic);
    // channel->broadcast(":" + client.getNickname() + " TOPIC " + channelName + " :" + topic);
}