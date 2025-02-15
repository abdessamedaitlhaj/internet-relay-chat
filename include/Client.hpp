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
        std::string _nickName;
        std::string _userName;
        std::string _hostName;
        std::string _realName;
        int _level;
        std::string _buffer;
        std::string _ipAddress;
        std::vector<Channel*> _channels;
        std::vector<std::string> InviteChannels;
        bool _started;
    public:
        Client();
        Client(int fd);
        ~Client();
        void        setStarted(bool b);
        int         getFd() const;
        bool        isRegistered() const;
        bool        getInviteChannel(const std::string& name) const;
        std::string getPassword() const;
        std::string getNickName() const;
        std::string getUserName() const;
        std::string getHostName() const;
        std::string getRealName() const;
        std::string getIpAddress() const;
        std::string getBuffer() const;
        int         getLevel() const ;
        void        setLevel(int number);
        bool        getStarted() const ;
        void        addLevel();
        void        setFd(int fd);
        void        setRegistered(bool registered);
        void        setStart();
        void        setPassword(const std::string& password);
        void        setNickName(const std::string& nickname);
        void        setUserName(const std::string& username);
        void        setRealName(const std::string& realname);
        void        setIpAddress(const std::string& ipAddress);
        void        setBuffer(const std::string& buffer);
        void        clearBuffer();
        void        addChannel(Channel* channel);
        void        removeChannelInvite(std::string& name);
        void        addInviteChannel(std::string& name);
};
