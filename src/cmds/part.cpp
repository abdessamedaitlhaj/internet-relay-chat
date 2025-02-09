#include "../..//include/Server.hpp"


void Server::handlePart(int fd, std::string &input, Client &client) {

    // std::vector<std::string> tokens;

    // tokens = Server::split(input, std::string("\t "));

    // if (tokens.size() < 2) {
    //     sendResponse(fd, ERR_NEEDMOREPARAMS(client.getNickName(), tokens[0]));
    //     return ;
    // }

    // std::vector<std::string> channels = Server::split(tokens[1], std::string(","));
    // std::string reason = tokens.size() > 2 ? tokens[2] : "Leaving";

    // for (size_t i = 0; i < channels.size(); i++) {
    //     std::string name = channels[i];
    //     if (name.empty() || name[0] != '#') {
    //         sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), name));
    //         continue ;
    //     }
    //     std::string channelName = name.substr(1);
    //     Channel *channel = getChannel(channelName);
    //     if (!channel) {
    //         sendResponse(fd, ERR_NOSUCHCHANNEL(client.getNickName(), channelName));
    //         continue ;
    //     }
    //     if (!channel->isMember(&client)) {
    //         sendResponse(fd, ERR_NOTONCHANNEL(client.getNickName(), channelName));
    //         continue ;
    //     }
    //     channel->removeMember(&client);
    //     if (channel->getclientsnumber() == 0) {
    //         for (size_t i = 0; i < _channels.size(); i++) {
    //             if (_channels[i] == channel) {
    //                 _channels.erase(_channels.begin() + i);
    //                 break ;
    //             }
    //         }
    //         delete channel;
    //     }
    //     std::string partReply = client.getHostName() + client.getIpAddress() + " PART #" + channelName + " :" + reason + CRLF;
    //     channel->broadcastToAll(partReply);
    // }
    ;
}