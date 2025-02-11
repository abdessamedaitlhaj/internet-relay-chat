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
            if ((modes[i] == 'k' || modes[i] == 'l' || modes[i] == 'o') && !args.empty()) {
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

std::string reduceSignedString(const std::string& input) {
    if (input.empty()) return "";
    
    std::string result;
    std::vector<char> currentChars;
    char currentSign = 0;
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];

        if (c == '+' || c == '-') {
            if (currentSign != 0 && c != currentSign) {
                result += currentSign;
                for (size_t j = 0; j < currentChars.size(); j++) {
                    result += currentChars[j];
                }
                currentChars.clear();
            }
            currentSign = c;
        } else {
            currentChars.push_back(c);
        }
    }
    if (!currentChars.empty()) {
        result += currentSign;
        for (size_t i = 0; i < currentChars.size(); i++) {
            result += currentChars[i];
        }
    }
    
    return result;
}

std::string accumulateStrings(std::vector<std::string> params) {

    std::string result;

    for (size_t i = 0; i < params.size(); ++i) {
        if (i == 0) {
            result += params[i];
        } else {
            result += " " + params[i];
        }
    }
    return result;
}

std::string Server::getAppliedModes(std::vector<ModeChange>& modeChanges, Channel &channel) {
    std::vector<char> modes;
    std::string result;
    std::vector<std::string> params;
    std::string finalModes;
    for (size_t i = 0; i < modeChanges.size(); ++i) {
        if (modeChanges[i].mode == 'o' || modeChanges[i].mode == 'i' || modeChanges[i].mode == 't' || modeChanges[i].mode == 'k' || modeChanges[i].mode == 'l') {
            if (modeChanges[i].sign && modeChanges[i].sign == '+') {
                modes.push_back('+');
                if (!modeChanges[i].argument.empty() && (modeChanges[i].mode == 'k' || modeChanges[i].mode == 'l' || modeChanges[i].mode == 'o')) {
                    if (modeChanges[i].mode == 'l' && modeChanges[i].argument.find_first_not_of("0123456789") == std::string::npos && modeChanges[i].argument.find("-") == std::string::npos) {
                        modes.push_back('l');
                        params.push_back(modeChanges[i].argument);
                    }
                    else if (modeChanges[i].mode == 'o' && getClientNick(modeChanges[i].argument)) {
                        modes.push_back('o');
                        params.push_back(modeChanges[i].argument);
                    }
                    else if (modeChanges[i].mode == 'k') {
                        modes.push_back('k');
                        params.push_back(modeChanges[i].argument);
                    }
                } else if (modeChanges[i].mode == 'i' || modeChanges[i].mode == 't') {
                    modes.push_back(modeChanges[i].mode);
                }
            }
            else if (modeChanges[i].sign && modeChanges[i].sign == '-') {
                modes.push_back('-');
                if (modeChanges[i].mode == 'k' && !channel.getPassword().empty())
                    params.push_back("*");
                std::string tmp = getAllModes(&channel);
                std::vector<std::string> exitingModes = split(tmp, "\t ");
                if (exitingModes.size() && exitingModes[0].find("itlok") != std::string::npos) 
                    modes.push_back(modeChanges[i].mode);
            }
        }
    }
    std::string modesToString(modes.begin(), modes.end());
    result = reduceSignedString(modesToString);
    std::string paramsToString = accumulateStrings(params);
    result += " " + paramsToString;
    return result;
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

    std::vector<ModeChange> modeChanges = parseModes(tokens[2], args);

    for (size_t i = 0; i < modeChanges.size(); ++i) {
        if (modeChanges[i].mode == 'i' && modeChanges[i].sign) {
            channel->setInviteOnly(modeChanges[i].sign == '+' ? true : false);
        } else if (modeChanges[i].mode == 't' && modeChanges[i].sign) {
            channel->setTopicRestriction(modeChanges[i].sign == '+' ? true : false);
        } else if (modeChanges[i].mode == 'l' && !modeChanges[i].argument.empty()) {
            if (modeChanges[i].argument.find_first_not_of("0123456789") == std::string::npos && modeChanges[i].argument.find("-") == std::string::npos) {
                std::cout << std::stoi(modeChanges[i].argument) << std::endl;
                channel->setLimit(std::stoi(modeChanges[i].argument));
            } else
                continue ;
            channel->setUserLimit(modeChanges[i].sign == '+' ? true : false);
        } else if (modeChanges[i].mode == 'k' && !modeChanges[i].argument.empty()) {
            channel->setAuth(modeChanges[i].sign == '+' ? true : false);
            if (modeChanges[i].sign == '+') {
                channel->setPassword(modeChanges[i].argument);
            }
        } else if (modeChanges[i].mode == 'o' && !modeChanges[i].argument.empty()) {
            Client *op = getClientNick(modeChanges[i].argument);
            if (op) {
                if (modeChanges[i].sign == '+') {
                    channel->addOperator(op);
                } else {
                    channel->removeOperator(op);
                }
            } else {
                sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), modeChanges[i].argument));
            }
        }
        if (modeChanges[i].mode != 'o' && modeChanges[i].mode != 'l' && modeChanges[i].mode != 'k' && modeChanges[i].mode != 't' && modeChanges[i].mode != 'i') {
            sendResponse(fd, ERR_UNKNOWNMODE(client.getNickName(), std::string(1, modeChanges[i].mode)));
        }
    }
    std::string appliedModes = getAppliedModes(modeChanges, *channel);
    if (!appliedModes.empty()) {
        std::string response = ":" + client.getHostName() + client.getIpAddress() + " MODE #" + channelName + " " + appliedModes + CRLF;
        channel->broadcastToAll(response);
    }
}