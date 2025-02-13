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
            if ( !args.empty() && (modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o') && sign == '+') {
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

std::string accumulateStrings(std::map<char, std::string> &params) {

    std::string result;
    std::map<char, std::string>::iterator it = params.begin();

    while (it != params.end()) {
        result += it->second;
        ++it;
        if (it != params.end())
            result += " ";
    }
    return result;
}


std::string Server::checkModes(int fd, ModeChange &modeChange, Channel &channel, std::map<char, std::string> &params) {

    Client *client = getClient(fd);
    std::string add;
    std::string remove;
    std::string channelName = channel.getName();

    if (modeChange.mode == 'i' && modeChange.sign) {
        if (!channel.getInviteOnly() && modeChange.sign == '+') {
            channel.setInviteOnly(true);
            add += "i";
        }
        if (channel.getInviteOnly() && modeChange.sign == '-') {
            channel.setInviteOnly(false);
            remove += "i";
        }
    } else if (modeChange.mode == 't' && modeChange.sign) {
        if (!channel.getTopicRestriction() && modeChange.sign == '+') {
            channel.setTopicRestriction(true);
            add += "t";
        }
        if (channel.getTopicRestriction() && modeChange.sign == '-') {
            channel.setTopicRestriction(false);
            remove += "t";
        }
    }
    else if (modeChange.sign && modeChange.mode == 'l') {
        if (modeChange.sign == '+' && !modeChange.argument.empty() && \
            modeChange.argument.find_first_not_of("0123456789") == std::string::npos && \
            modeChange.argument.find("-") == std::string::npos) {
            channel.setLimit(std::atoi(modeChange.argument.c_str()));
            channel.setUserLimit(true);
            add += "l";
            params['l'] = modeChange.argument;
        } else if (modeChange.sign == '-' && channel.getUserLimit()) {
                channel.setUserLimit(false);
                channel.setLimit(0);
                remove += "l";
        }
    } else if (modeChange.sign && modeChange.mode == 'k') {
        if (modeChange.sign == '+' && !modeChange.argument.empty()) {
            channel.setAuth(true);
            channel.setPassword(modeChange.argument);
            add += "k";
            params['k'] = modeChange.argument;
        } else if (modeChange.sign == '-' && channel.getAuth()) {
            channel.setAuth(false);
            channel.setPassword("");
            remove += "k";
        }
    } else if (modeChange.sign && modeChange.mode == 'o') {
        if (!modeChange.argument.empty()) {
            Client *op = getClientNick(modeChange.argument);
            if (op && channel.isMember(op)) {
                if (modeChange.sign == '+' && fd != op->getFd()) {
                    channel.addOperator(op);
                    add += "o";
                    params['o'] = modeChange.argument;
                } else {
                    channel.removeOperator(op);
                    remove += "o";
                }
            }
            else {
                if (op && !channel.isMember(op))
                    sendResponse(fd, ERR_NOTONCHANNEL(client->getNickName(), channelName));
                else
                    sendResponse(fd, ERR_NOSUCHNICK(client->getNickName(), modeChange.argument));
            }
        }
    }
    std::sort(add.begin(), add.end());
    std::sort(remove.begin(), remove.end());
    if (!add.empty())0
        add.insert(0, "+");
    if (!remove.empty())
        remove.insert(0, "-");
    return add + remove;
}

std::string reduce_mode(const std::string &mode) {
    std::string result;
    bool plus_section = false, minus_section = false;
    bool plus_seen[256] = {false};
    bool minus_seen[256] = {false};

    for (std::size_t i = 0; i < mode.size(); ++i) {
        char c = mode[i];

        if (c == '+') {
            if (!plus_section) result += c;
            plus_section = true;
            minus_section = false;
        } else if (c == '-') {
            if (!minus_section) result += c;
            plus_section = false;
            minus_section = true;
        } else {
            if (plus_section && !plus_seen[(unsigned char)c]) {
                result += c;
                plus_seen[(unsigned char)c] = true;
            }
            if (minus_section && !minus_seen[(unsigned char)c]) {
                result += c;
                minus_seen[(unsigned char)c] = true;
            }
        }
    }
    return result;
}

std::string Server::applyModes(int fd, std::vector<ModeChange>& modeChanges, Channel &channel) {

    std::string modes;
    std::map<char, std::string> modeArgs;
    Client *client = getClient(fd);

    std::string channelName = channel.getName();
    for (size_t i = 0; i < modeChanges.size(); ++i) {
        modes += checkModes(fd, modeChanges[i], channel, modeArgs);
        if (modeChanges[i].mode != 'o' && modeChanges[i].mode != 'l' && modeChanges[i].mode != 'k' && modeChanges[i].mode != 't' && modeChanges[i].mode != 'i')
            sendResponse(fd, ERR_UNKNOWNMODE(client->getNickName(), std::string(1, modeChanges[i].mode)));
    }

    modes = reduce_mode(modes);
    std::string paramsToString = accumulateStrings(modeArgs);
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
    if (!channel->isOperator(&client)) {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channelName));
        return;
    }
    for (size_t i = 3; i < tokens.size(); ++i)
        args.push(tokens[i]);

    std::vector<ModeChange> modeChanges = parseModes(tokens[2], args);

    // print mode chnages
    for (size_t i = 0; i < modeChanges.size(); ++i) {
        std::cout << "Mode: " << modeChanges[i].mode << " Sign: " << modeChanges[i].sign << " Arg: " << modeChanges[i].argument << std::endl;
    }

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