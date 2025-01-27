#pragma once

#include "../include/Channel.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>

class Client;
class Channel;

class Channel {

    private:
        bool _inviteOnly;
        bool _topicRestriction;
        bool _userLimit;
        std::map<char, bool> _modes;
        std::string _name;
        std::string _topic;
        std::vector<Client> _members;
        std::vector<Client> _operators;
        std::vector<Channel> _channels;
        void sendToClient(Client *client, const std::string &message) const;

    public:
        Channel(const std::string &name);
        ~Channel();

        std::string getName() const;
        std::string getTopic() const;

        void setTopic(const std::string &topic);

        void addMember(Client *client);
        void removeMember(Client *client);
        void addOperator(Client *client);
        void removeOperator(Client *client);
        bool isMember(Client *client) const;
        bool isOperator(Client *client) const;

        void setMode(char mode, bool enable);
        bool hasMode(char mode) const;

        void broadcast(const std::string &message, Client *sender = nullptr) const;
};
