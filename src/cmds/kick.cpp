#include "../../include/Server.hpp"

void Server::handleKick(int fd, std::string &input, Client& client)
{
    std::vector<std::string> tokens;

    tokens = Server::split(input, std::string("\t "));
    if (tokens.size() < 3)
    {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return ;
    }
    std::string channelname = tokens[1];
    std::vector<std::string> nick_tokens;
    size_t i = tokens[2].find(",");
    if (i != std::string::npos)
        nick_tokens = Server::split(tokens[2], ",");
    else
        nick_tokens.push_back(tokens[2]);
    std::string reason;
    if (tokens.size() > 3)
        reason = tokens[3];
    else
        reason = "kicked";
    // validate channel name
    if (channelname.empty() || channelname[0] != '#' )
    {
        sendResponse(fd, ERR_BADCHANMASK(channelname));
        return ;
    }
    std::string channel_name = channelname.substr(1);
    for (size_t i = 0; i < nick_tokens.size(); i++)
    {
        std::string nickname = nick_tokens[i];
        Channel *channel = getChannel(channel_name);
        if (!channel)
        {
            sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channel_name));
            return ;
        }
        //check if sender is not on the channel
        if (!channel->isMember(&client))
        {
            sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channel_name));
            return ;
        }
        Client *new_user = getClientNick(nickname);
        //check if user is not on the channel
        if (!new_user || !channel->isMember(new_user))
        {
            sendResponse(fd, ERR_USERNOTINCHANNEL(client.getNickName(), nickname, channel_name));
            return ;
        }
        //check if sender is operator
        if (!channel->isOperator(&client))
        {
            sendResponse(fd, ERR_CHANOPRIVSNEEDED(client.getNickName(), channel_name));
            return ;
        }
        std::string response = ":" + client.getHostName() + client.getIpAddress() + " KICK " + channelname + " " + nickname;
        if (reason.empty())
            response += CRLF;
        else
            response += " :" + reason + CRLF;
        channel->broadcastToAll(response);
        channel->removeMember(new_user);
    }
}