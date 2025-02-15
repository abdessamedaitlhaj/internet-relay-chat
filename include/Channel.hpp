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
        bool _auth;
        int _limit;
        std::map<char, bool> _modes;
        std::string _name;
        std::string _topic;
        std::vector<Client> _members;
        std::vector<Client> _operators;
        std::string _password;
        time_t _topicTime;

    public:
        Channel(const std::string &name);
        ~Channel();
        std::string getName() const;
        std::string getTopic() const;

        int         getlimit() const;
        bool        getTopicRestriction() const;
        bool        getInviteOnly() const;
        bool        getUserLimit() const;
        bool        getAuth() const;
        std::string getPassword() const;
        int         getLimit() const;
        bool        getMode(char mode) const;
        void        setInviteOnly(bool inviteOnly);
        void        setAuth(bool auth);
        void        setLimit(int limit);
        void        setUserLimit(bool userLimit);
        time_t      getTopicTime() const;
        int         getclientsnumber() const;
        void        setPassword(const std::string &password);
        void        setTopicRestriction(bool topicRestriction);
        void        setname(const std::string &name);
        void        setTopic(const std::string &topic);
        void        setTopicTime(time_t topicTime);
        void        addMember(Client *client);
        void        removeMember(Client *client);
        void        addOperator(Client *client);
        void        removeOperator(Client *client);
        bool        isMember(Client *client);
        bool        isOperator(Client *client) const;
        bool        isInvited(Client *client, std::string name) const;
        void        setMode(char mode, bool enable);
        bool        hasMode(char mode) const;
        void        broadcast(const std::string &message, Client *sender) const;
        std::string ChannelsclientList();
        void        broadcastToAll(const std::string &message) const;
        Client		*getclient(std::string name);
};
