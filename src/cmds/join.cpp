#include "../../include/Server.hpp"
#include "../../include/numericReplies.hpp"
#include "../../include/Channel.hpp"

void Server::handleJoin(int fd, std::vector<std::string>& tokens, std::string& trailing, Client& client)
{
    //need more params
    if (tokens.size() < 2)
    {
        sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
        return ;
    }
    // if (tokens[1] == "0")
    // {
    //     std::vector<std::string> userchannels = client.getChannels();
    // }
    std::vector<std::pair<std::string, std::string > > pairs_channels;
    std::string channels= tokens[1];
    std::string passwords = "";
    if (tokens.size() > 2)
        passwords = tokens[2];
    std::string buffer;
    //split channels
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
    //split passwords
    if (!passwords.empty())
    {
        buffer.clear();
        for (size_t i = 0; i < passwords.length(); i++)
        {
            if (passwords[i] == ',')
            {
                pairs_channels[pairs_channels.size() - 1].second = buffer;
                buffer.clear();
            }
            else
                buffer += passwords[i];
        }
        pairs_channels[pairs_channels.size() - 1].second = buffer;
    }
    //channel processing
    for (size_t i = 0; i < pairs_channels.size(); i++)
    {
        std::string channel_name = pairs_channels[i].first;
        std::string password = pairs_channels[i].second;
        // if (channel_name.empty() || (channel_name[0] != '#' && channel_name[0] != '&'))
        if (channel_name[0] != '#' && channel_name[0] != '&')
        {
            sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channel_name));
            continue ;
        }
        Channel *channel = getChannel(channel_name);
        if (!channel)
        {
            // channel = new Channel(channel_name);
            // _channels.push_back(channel);
            // // add user
            // // make him the admin of the channel
            // if (channel->isoperator(&client))
        }
        else
        {
            if (channel->isMember(&client))
                continue ;
            if (channel->getInviteOnly() && !channel->isInvited(&client, channel_name, 1))
            {
                sendResponse(fd, ERR_INVITEONLYCHAN(client.getNickName(), channel_name));
                continue ;
            }
        }
    }
}
