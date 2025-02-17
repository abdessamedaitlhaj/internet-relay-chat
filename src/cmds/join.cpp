#include "../../include/Server.hpp"

void Server::handleJoin(int fd, std::string &input, Client& client)
{
    std::vector<std::string> tokens;
    tokens = Server::split(input, std::string("\t "));
    if (tokens.size() < 2)
    {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return ;
    }
    std::vector<std::pair<std::string, std::string > > pairs_channels;
    std::string channels= tokens[1];
    std::string passwords = "";
    if (tokens.size() > 2)
        passwords = tokens[2];
    std::string buffer;
    for (size_t i = 0; i < channels.length(); i++)
    {
        if (channels[i] == ',')
        {
            pairs_channels.push_back(std::make_pair(buffer, ""));
            buffer.clear();
        }
        else
            buffer += channels[i];
    }
    pairs_channels.push_back(std::make_pair(buffer, ""));
    if (!passwords.empty()) {
        buffer.clear();
        size_t j = 0;
        for (size_t i = 0; i < passwords.length(); i++) {
            if (passwords[i] == ',') {
                if (j < pairs_channels.size())
                    pairs_channels[j].second = buffer;
                j++;
                buffer.clear();
            } else {
                buffer += passwords[i];
            }
        }
        if (j < pairs_channels.size())
            pairs_channels[j].second = buffer;
    }
    //channel processing
    for (size_t i = 0; i < pairs_channels.size(); i++)
    {
        std::string channel_name = pairs_channels[i].first;
        std::string password = pairs_channels[i].second;
        if (channel_name.empty() || channel_name[0] != '#')
        {
            sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channel_name));
            continue ;
        }
        std::string _channel_name = channel_name.substr(1);
        Channel *channel = getChannel(_channel_name);
        if (!channel)
        {
            Channel *new_channel = new Channel(_channel_name);
            new_channel->addOperator(&client);
            _channels.push_back(new_channel);
            new_channel->addMember(&client);
            channel = new_channel;
        }
        else
        {
            if (channel->isMember(&client))
            {
                continue ;
            }
            if (!channel->getPassword().empty() && channel->getPassword() != password)
            {
                if (!channel->isInvited(&client, _channel_name))
                {
                    sendResponse(fd, ERR_BADCHANNELKEY(client.getNickName(), _channel_name));
                    continue ;
                }
            }
            if (channel->getInviteOnly())
            {
                if(!channel->isInvited(&client, _channel_name))
                {
                    sendResponse(fd, ERR_INVITEONLYCHAN(client.getNickName(), _channel_name));
                    continue ;
                }
                client.removeChannelInvite(_channel_name);
            }
            if (channel->getlimit() && (channel->getlimit() <= channel->getclientsnumber()))
            {
                sendResponse(fd, ERR_CHANNELISFULL(client.getNickName(), _channel_name));
                continue ;
            }
            channel->addMember(&client);
        }
        std::string joinreply = RPL_JOINMSG(client.getHostName(), client.getIpAddress(), _channel_name);
        if (!channel->getTopic().empty()) {
            std::string topicreply = ": 332 " + client.getNickName() + " #" + _channel_name + " :" + channel->getTopic() + "\r\n";
            joinreply += topicreply;
        }
        std::string namereply = RPL_NAMREPLY(client.getNickName(), _channel_name, channel->ChannelsclientList());
        std::string endofnames = RPL_ENDOFNAMES(client.getNickName(), _channel_name);
        sendResponse(fd, joinreply + namereply + endofnames);
        channel->broadcast(RPL_JOINMSG(client.getHostName(), client.getIpAddress(), _channel_name), &client);
    }
}
