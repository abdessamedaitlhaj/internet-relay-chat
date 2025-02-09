#include "../../include/Server.hpp"

std::string Server::getMsg(std::vector<std::string> &tokens) {
    std::string msg;
    for (size_t i = 2; i < tokens.size(); ++i) {
        msg += tokens[i] + " ";
    }
    return msg;
}

void    Server::handlePrivmsg(int fd, std::string &input, Client &client) {

    // input

    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));

    if (tokens.size() == 1) {
        sendResponse(fd, ERR_NORECIPIENT(client.getNickName()));
        return;
    } else if (tokens.size() == 2) {
        sendResponse(fd, ERR_NOTEXTTOSEND(client.getNickName()));
        return;
    }

    std::vector<std::string> targts = split(tokens[1], std::string(","));
    std::string target;
    std::string last;
    size_t pos;
    std::string trailing;
    std::string response;
    for (size_t i = 0; i < targts.size(); ++i) {
        target = targts[i];
        if (target[0] == '#') {
            std::string channelName = target.substr(1);
            if (!getChannel(channelName)) {
                sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), "#" + channelName));
                continue ;
            }
            if (tokens.size() > 3) {
                trailing = getMsg(tokens);
            }
            else {
                last = tokens[2];
                pos  = last.find_first_of(":");
                trailing = pos != std::string::npos ? last.substr(pos + 1) : last;
            }
            Channel *channel = getChannel(channelName);
            if (!channel->isMember(&client)) {
                sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
                continue ;
            }
            response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG #" + channelName + " :" + trailing + CRLF;
            channel->broadcast(response, &client);
        }
        else {
            std::string userName;
            size_t pos = target.find_first_of("@");
            if (pos != std::string::npos) {
                std::string serverName = target.substr(pos + 1);
                if (serverName != client.getIpAddress() && serverName != "localhost" && client.getIpAddress() != "127.0.0.1") {
                    sendResponse(fd, ERR_NOSUCHSERVER(client.getNickName(), serverName));
                    continue ;
                }
                if (target.find_first_of("!") == std::string::npos)
                    userName = target.substr(0, pos);
                else
                    target = target.substr(0, target.find_first_of("!"));
            } else if (target.find_first_of("!") != std::string::npos) {
                target = target.substr(0, target.find_first_of("!"));
            }
            Client *cli;
            if (userName.empty()) {
            std::cout << "ch : " << targts[i] << std::endl;
                cli = getClientNick(target);
                if (!cli) {
                    sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), target));
                    continue ;
                }
            }
            else {
                cli = getClientUserName(userName);
                if (!cli) {
                    sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), target));
                    continue ;
                }
            }
            response = ":" + client.getHostName() + client.getIpAddress() + " PRIVMSG " + target + " :" + trailing + CRLF;
            sendResponse(cli->getFd(), response);
        }
        }
    }