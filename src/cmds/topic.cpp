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

void Server::handleTopic(int fd, std::string &input, Client &client) {

    size_t trailingPos;
    std::string trailing;

    trailingPos = input.find_first_of(":");
    trailing = trailingPos != std::string::npos ? input.substr(trailingPos + 1) : "";
    if (!trailing.empty()) {
        input = input.substr(0, trailingPos);
    }
    std::vector<std::string> tokens = split(input, std::string("\t "));

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return;
    }

    if (!channelNameValid(tokens[1])) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), tokens[1].substr(1)));
        return;
    }
    std::string channelName = tokens[1].substr(1);
    addChannel(new Channel(std::string("d")));
    Channel *channel = getChannel(channelName);
    if (!channel) {
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channelName));
        return;
    }
    channel->addMember(&client);
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
        return;
    }
    if (tokens.size() == 2) {
        if (channel->getTopic().empty()) {
            sendResponse(fd, RPL_NOTOPIC(client.getNickName(), client.getHostName(), client.getIpAddress(), channelName));
        } else {
            sendResponse(fd, RPL_TOPIC(client.getNickName(), client.getHostName(), client.getIpAddress(), channelName, channel->getTopic()));
        }
        return;
    }
    if (channel->hasMode('t') && !channel->isOperator(&client)) {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    std::string last;
    size_t pos;
    if (tokens.size() > 3) {
        trailing = getMsg(tokens);
    }
    else {
        last = tokens[2];
        pos  = last.find_first_of(":");
        trailing = pos != std::string::npos ? last.substr(pos + 1) : last;
    }
    channel->setTopic(trailing);
    std::string response = ":" + client.getHostName() + client.getIpAddress() + " TOPIC #" + channelName + " :" + trailing + CRLF;
    channel->broadcast(response, &client);
    channel->broadcast(RPL_TOPICWHOTIME(client.getNickName(), channelName, client.getNickName(), std::to_string(channel->getTopicTime())), &client);
}