#include "../../include/Server.hpp"

void Server::handleQuit(int fd, std::string &input, Client& client)
{
    std::vector<std::string> tokens;
    tokens = Server::split(input, std::string("\t "));
    std::string reason = "Quit";
    if (tokens.size() > 1)
    {
        size_t pos = input.find_first_of(":");
        if (pos != std::string::npos)
        {
            reason = "Quit: ";
            reason += input.substr(pos + 1);
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
                std::string response = ":" + client.getHostName() + client.getIpAddress() +  " QUIT " + ":" + reason + CRLF;
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
    removeClient(fd);
    removeFd(fd);
    close(fd);
}
