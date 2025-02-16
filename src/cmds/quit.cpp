#include "../../include/Server.hpp"

void Server::handleQuit(int fd, std::string &input, Client& client)
{
    std::vector<std::string> tokens;
    tokens = Server::split(input, std::string("\t "));
    std::string reason = "Quit";
    if (tokens.size() > 1)
    {
        if (tokens[1][0] == ':')
        {
            tokens[1] = tokens[1].substr(1);
            reason = "";
            for (size_t i = 1; i < tokens.size(); i++)
            {
                reason += tokens[i];
                if (i < tokens.size() - 1)
                    reason += " ";
            }
        }
        else
            reason = tokens[1];
    }
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->getclient(client.getNickName()))
        {
            if (_channels[i]->isOperator(&client))
                _channels[i]->removeOperator(&client);
            _channels[i]->removeMember(&client);
            if (_channels[i]->getclientsnumber() != 0)
            {
                std::string response = ":" + client.getHostName() + " QUIT " + ":" + reason + CRLF;
                _channels[i]->broadcastToAll(response);
            }
            else
            {
                _channels.erase(_channels.begin() + i);
                delete _channels[i];
                i--;
            }
        }
    }
    std::cout << RED << "Client <" << fd << "> Disconnected" << RESET << std::endl;
    removeFd(fd);
    close(fd);
}
