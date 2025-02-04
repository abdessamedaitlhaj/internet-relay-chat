#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Channel.hpp" 

class Channel;

class Client {

    private:
        int _fd;
        bool _registered;
        std::string _password;
        std::string _nickname;
        std::string _username;
        std::string _hostname;
        std::string _relaname;
        std::string _buffer;
        std::vector<Channel*> _channels;

    public:
        Client();
        Client(int fd);
        ~Client();

        int getFd() const;
        bool isRegistered() const;
        bool getinvitechannel(const std::string& name) const;
        std::string getPassword() const;
        std::string getNickname() const;
        std::string getUsername() const;
        std::string getHostname() const;
        std::string getRealname() const;
        std::string getBuffer() const;

        void setFd(int fd);
        void setRegistered(bool registered);
        void setPassword(const std::string& password);
        void setNickName(const std::string& nickname);
        void setUserName(const std::string& username);
        void setHostName(const std::string& hostname);
        void setRealName(const std::string& realname);
        void setBuffer(const std::string& buffer);
        void clearBuffer();
        void addChannel(Channel* channel);
        void removeChannelInvite(std::string& name);
};
