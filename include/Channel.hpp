#pragma once

#include "Client.hpp"
#include <string>
#include <vector>
#include <set>
#include <map>
#include <sys/socket.h>

class Client;

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
        time_t _topicTime;
        
        //void sendToClient(Client *client, const std::string &message) const;

    public:
        Channel(const std::string &name);
        ~Channel();

        std::string getName() const;
        std::string getTopic() const;
        bool getTopicRestriction() const;
        bool getInviteOnly() const;
        bool getMode(char mode) const;
        time_t getTopicTime() const;

        void setTopicRestriction(bool topicRestriction);

        void setTopic(const std::string &topic);
        void setTopicTime(time_t topicTime);
        

        void addMember(Client *client);
        void removeMember(Client *client);
        void addOperator(Client *client);
        void removeOperator(Client *client);
        bool isMember(Client *client) const;
        bool isOperator(Client *client) const;
        bool isInvited(Client *client, std::string name, int flg) const;

        void setMode(char mode, bool enable);
        bool hasMode(char mode) const;

        void broadcast(const std::string &message, Client *sender) const;
};
