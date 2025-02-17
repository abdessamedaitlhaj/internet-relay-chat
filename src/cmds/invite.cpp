#include "../../include/Server.hpp"

void Server::handleInvite(int fd, std::string &input, Client &client)
{
    std::vector<std::string> tokens;
    tokens = Server::split(input, std::string("\t "));
    if (tokens.size() < 3)
    {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return ;
    }
    std::string nickname = tokens[1];
    std::string channel_name = tokens[2].substr(1);
    Channel *channel = getChannel(channel_name);
    if (!channel) 
    {
        sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channel_name));
        return ;
    }
    if (!channel->isMember(&client))
    {
        sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channel_name));
        return ;
    }
    if (channel->getInviteOnly() && !channel->isOperator(&client))
    {
        sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channel_name));
        return ;
    }
    Client *new_user = getClientNick(nickname);
    if (!new_user)
    {
        sendResponse(fd, ERR_NOSUCHNICK(client.getNickName(), nickname));
        return ;
    }
    if (channel->isMember(new_user))
    {
        sendResponse(fd, ERR_USERONCHANNEL(client.getNickName(), new_user->getNickName(), channel_name));
        return ;
    }
    new_user->addInviteChannel(channel_name);
    sendResponse(fd, RPL_INVITING(client.getNickName(), new_user->getNickName(), channel_name));
    std::string response = ":" + client.getHostName() + " INVITE " + new_user->getNickName() + " #" + channel_name + CRLF;
    sendResponse(new_user->getFd(), response);
}