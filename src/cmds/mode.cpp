#include "../..//include/Server.hpp"

std::vector<ModeChange> parseModes(const std::string& modes, std::queue<std::string>& args) {
    std::vector<ModeChange> changes;
    char sign = '\0';

    for (size_t i = 0; i < modes.length(); ++i) {
        if (modes[i] == '+') {
            sign = '+';
        } else if (modes[i] == '-') {
            sign = '-';
        } else {
            std::string arg;
            if ( !args.empty() && ((modes[i] == 'l' && sign == '+') || (modes[i] == 'k' && sign == '+') || (modes[i] == 'o' && sign))) {
                arg = args.front();
                args.pop();
            }
            changes.push_back((ModeChange){modes[i], sign, arg});
        }
    }
    return changes;
}

std::string getAllModes(Channel *channel) {
    std::string modes;
    std::string params;

    if (channel->getInviteOnly()) {
        modes += "i";
    }
    if (channel->getAuth()) {
        modes += "k";
        params = channel->getPassword();
    }
    if (channel->getUserLimit()) {
        modes += "l";
        if (!params.empty())
            params += " ";
        params += std::to_string(channel->getLimit());
    }
    if (channel->getTopicRestriction()) {
        modes += "t";
    }
    if (modes.length() > 0) {
        modes.insert(0, "+");
    }
    if (!params.empty())
        modes.append(" " + params);
    return modes;
}

std::string accumulateStrings(std::map<char, std::string> &params, std::vector<std::string> &opsParams) {

    std::string result;
    std::map<char, std::string>::iterator it = params.begin();

    while (it != params.end()) {
        result += it->second;
        ++it;
        if (it != params.end())
            result += " ";
    }
    if (!result.empty() && !opsParams.empty())
        result += " ";
    for (size_t i = 0; i < opsParams.size(); ++i) {
        result += opsParams[i];
        if (i != opsParams.size() - 1)
            result += " ";
    }
    return result;
}

std::string toStrings(std::set<char> &add, std::set<char> &remove, std::vector<char> &addOps, std::vector<char> &removeOps) {

    std::string plus;
    std::string minus;
    if (!add.empty()) {
        plus += "+";
        for (std::set<char>::iterator it = add.begin(); it != add.end(); ++it) {
            plus += *it;
        }
    }
    if (!addOps.empty()) {
        if (add.empty())
            plus += "+";
        for (size_t i = 0; i < addOps.size(); ++i) {
            plus += addOps[i];
        }
    }
    if (!remove.empty()) {
        minus += "-";
        for (std::set<char>::iterator it = remove.begin(); it != remove.end(); ++it) {
            minus += *it;
        }
    }
    if (!removeOps.empty()) {
        if (remove.empty())
            minus += "-";
        for (size_t i = 0; i < removeOps.size(); ++i) {
            minus += removeOps[i];
        }
    }

    return (plus + minus);
}

std::string Server::applyModes(int fd, std::vector<ModeChange>& modeChanges, Channel &channel) {

    std::string modes;
    Client *client = getClient(fd);
    std::set<char> add;
    std::set<char> remove;
    std::map<char, std::string> params;
    std::vector<char> removeOps;
    std::vector<char> addOps;
    std::vector<std::string> opsParams;

    std::string channelName = channel.getName();
    for (size_t i = 0; i < modeChanges.size(); ++i) {
        if (modeChanges[i].mode == 'i' && modeChanges[i].sign) {
            if (!channel.getInviteOnly() && modeChanges[i].sign == '+') {
                channel.setInviteOnly(true);
                add.insert('i');
            }
            if (channel.getInviteOnly() && modeChanges[i].sign == '-') {
                channel.setInviteOnly(false);
                remove.insert('i');
            }
        } else if (modeChanges[i].mode == 't' && modeChanges[i].sign) {
            if (!channel.getTopicRestriction() && modeChanges[i].sign == '+') {
                channel.setTopicRestriction(true);
                add.insert('t');
            }
            if (channel.getTopicRestriction() && modeChanges[i].sign == '-') {
                channel.setTopicRestriction(false);
                remove.insert('t');
            }
        }
        else if (modeChanges[i].sign && modeChanges[i].mode == 'l') {
            if (modeChanges[i].sign == '+' && !modeChanges[i].argument.empty() && \
                modeChanges[i].argument.find_first_not_of("0123456789") == std::string::npos && \
                modeChanges[i].argument.find("-") == std::string::npos) {
                channel.setLimit(std::atoi(modeChanges[i].argument.c_str()));
                channel.setUserLimit(true);
                add.insert('l');
                params['l'] = modeChanges[i].argument;
            } else if (modeChanges[i].sign == '-' && channel.getUserLimit()) {
                    channel.setUserLimit(false);
                    channel.setLimit(0);
                    remove.insert('l');
            }
        } else if (modeChanges[i].sign && modeChanges[i].mode == 'k') {
            if (modeChanges[i].sign == '+' && !modeChanges[i].argument.empty()) {
                channel.setAuth(true);
                channel.setPassword(modeChanges[i].argument);
                add.insert('k');
                params['k'] = modeChanges[i].argument;
            } else if (modeChanges[i].sign == '-' && channel.getAuth()) {
                channel.setAuth(false);
                channel.setPassword("");
                remove.insert('k');
            }
        } else if (modeChanges[i].sign && modeChanges[i].mode == 'o' && !modeChanges[i].argument.empty()) {
                Client *op = getClientNick(modeChanges[i].argument);
                if (op && channel.isMember(op)) {
                    if (modeChanges[i].sign == '+' && !channel.isOperator(op)) {
                        channel.addOperator(op);
                        addOps.push_back('o');
                        opsParams.push_back(modeChanges[i].argument);
                    }
                    if (modeChanges[i].sign == '-' && channel.isOperator(op)) {
                        channel.removeOperator(op);
                        removeOps.push_back('o');
                        opsParams.push_back(modeChanges[i].argument);
                    }
                }
                else {
                    if (op && !channel.isMember(op))
                        sendResponse(fd, ERR_USERNOTINCHANNEL(client->getNickName(), modeChanges[i].argument, channelName));
                    else
                        sendResponse(fd, ERR_NOSUCHNICK(client->getNickName(), modeChanges[i].argument));
                }
        }
        if (modeChanges[i].mode != 'o' && modeChanges[i].mode != 'l' && modeChanges[i].mode != 'k' && modeChanges[i].mode != 't' && modeChanges[i].mode != 'i')
            sendResponse(fd, ERR_UNKNOWNMODE(client->getNickName(), std::string(1, modeChanges[i].mode)));
    }
    modes = toStrings(add, remove, addOps, removeOps);
    std::string paramsToString = accumulateStrings(params, opsParams);
    if (!paramsToString.empty())
        modes += " " + paramsToString;
    return modes;
}

void Server::handleMode(int fd, std::string &input, Client &client) {

    std::string exitingModes;
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
        exitingModes = getAllModes(channel);
        sendResponse(fd, RPL_CHANNELMODEIS(client.getNickName(), channelName, exitingModes));
        sendResponse(fd, RPL_CREATIONTIME(client.getNickName(), channelName, std::to_string(time(NULL))));
        return ;
    }
    if (!channel->isMember(&client)) {
        sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
        return;
    }
    if (!channel->isOperator(&client)) {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    for (size_t i = 3; i < tokens.size(); ++i)
        args.push(tokens[i]);

    std::vector<ModeChange> modeChanges = parseModes(tokens[2], args);

    std::string appliedModes = applyModes(fd, modeChanges, *channel);
    std::string response;
    if (appliedModes.empty()) {
        response = ":" + client.getHostName() + client.getIpAddress() + " MODE #" + channelName + " " + getAllModes(channel) + CRLF;
        sendResponse(fd, response);
    }
    else {
        response = ":" + client.getHostName() + client.getIpAddress() + " MODE #" + channelName + " " + appliedModes + CRLF;
        channel->broadcastToAll(response);
    }
}