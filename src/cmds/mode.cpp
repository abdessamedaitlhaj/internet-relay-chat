#include "../..//include/Server.hpp"


#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>

struct ModeChange {
    char mode;
    bool add;
    std::string argument;
};

std::vector<ModeChange> parseModes(const std::string& modes, std::queue<std::string>& args) {
    std::vector<ModeChange> changes;
    bool currentSign = true; // true for '+', false for '-'

    for (size_t i = 0; i < modes.size(); ++i) {
        if (modes[i] == '+') {
            currentSign = true;
        } else if (modes[i] == '-') {
            currentSign = false;
        } else {
            std::string arg;
            if ((modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o') && !args.empty()) {
                arg = args.front();
                args.pop();
            }
            changes.push_back((ModeChange){modes[i], currentSign, arg});
        }
    }
    return changes;
}

std::string getAllModes(Channel *channel) {
    std::string modes;
    if (channel->getInviteOnly()) {
        modes += "i";
    }
    if (channel->getTopicRestriction()) {
        modes += "t";
    }
    if (channel->getUserLimit()) {
        modes += "l";
    }
    if (channel->getAuth()) {
        modes += "k";
    }
    return modes;
}

void Server::handleMode(int fd, std::string &input, Client &client) {

    std::string modes;
    std::queue<std::string> args;
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
        modes = getAllModes(channel);
        sendResponse(fd, RPL_CHANNELMODEIS(client.getNickName(), channelName, modes));
        sendResponse(fd, RPL_CREATIONTIME(client.getNickName(), channelName, std::to_string(time(NULL))));
        return ;
    }
    if (!channel->isOperator(&client)) {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    for (size_t i = 3; i < tokens.size(); ++i)
        args.push(tokens[i]);

    std::vector<ModeChange> modeChanges = parseModes(modes, args);

    for (size_t i = 0; i < modeChanges.size(); ++i) {
        if (modeChanges[i].mode == 'i') {
            channel->setTopicRestriction(modeChanges[i].add);
        } else if (modeChanges[i].mode == 't') {
            channel->setTopicRestriction(modeChanges[i].add);
        } else if (modeChanges[i].mode == 'l' && !modeChanges[i].argument.empty()) {
            if (modeChanges[i].argument.find_first_not_of("0123456789") == std::string::npos && modeChanges[i].argument.find("-") == std::string::npos) {
                channel->setLimit(std::stoi(modeChanges[i].argument));
            } else
                continue ;
            channel->setUserLimit(modeChanges[i].add);
        } else if (modeChanges[i].mode == 'k' && !modeChanges[i].argument.empty()) {
            channel->setAuth(modeChanges[i].add);
            if (modeChanges[i].add) {
                channel->setPassword(modeChanges[i].argument);
            }
        } else if (modeChanges[i].mode == 'o' && !modeChanges[i].argument.empty()) {
            Client *op = getClientNick(modeChanges[i].argument);
            if (op) {
                if (modeChanges[i].add) {
                    channel->addOperator(op);
                } else {
                    channel->removeOperator(op);
                }
            }
        }
        std::string response = ":" + client.getHostName() + client.getIpAddress() + " MODE " + channelName + " " + getAllModes(channel);

    }
}