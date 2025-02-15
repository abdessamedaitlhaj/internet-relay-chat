#include "../include/Client.hpp"

// Constructor
Client::Client() : _fd(-1), _registered(false), _nickName(""), _userName(""), _hostName(""),_level(1), _buffer("")  {}

Client::Client(int fd) : _fd(fd), _registered(false), _nickName(""), _userName(""), _hostName(""),_level(1), _buffer("") , _started(false) {}

Client::~Client() {}

int Client::getFd() const {
    return _fd;
}

bool Client::isRegistered() const {
    return _registered;
}

bool Client::getInviteChannel(const std::string& name) const
{
    for (size_t i = 0; i < InviteChannels.size(); i++)
    {
        if (InviteChannels[i] == name)
            return true;
    }
    return false;
}

std::string Client::getPassword() const {
    return _password;
}

std::string Client::getNickName() const {
    return _nickName;
}

std::string Client::getUserName() const {
    return _userName;
}

std::string Client::getRealName() const {
    return _realName;
}

std::string Client::getHostName() const {
    return _nickName + "!~" + _userName + "@";
}

std::string Client::getIpAddress() const {
    return _ipAddress;
}

std::string Client::getBuffer() const {
    return _buffer;
}

void Client::setFd(int fd) {
    _fd = fd;
}

void Client::setRegistered(bool registered) {
    _registered = registered;
}

void Client::setPassword(const std::string& password) {
    _password = password;
}

void Client::setNickName(const std::string& nickName) {
    _nickName = nickName;
}

void Client::setUserName(const std::string& userName) {
    _userName = userName;
}

void Client::setRealName(const std::string& realName) {
    _realName = realName;
}

void Client::setIpAddress(const std::string& ipAddress) {
    _ipAddress = ipAddress;
}

void Client::setBuffer(const std::string& buffer) {
    _buffer += buffer;
}

void Client::setStarted(bool b) {
    _started = b;
}

bool Client::getStarted() const {
   return _started;
}

int Client::getLevel() const {
   return _level;
}
void  Client::setLevel(int number)  {
    _level = number;
}

void Client::clearBuffer() {

    _buffer.clear();
}

void Client::removeChannelInvite(std::string &name) {
    for (size_t i = 0; i < InviteChannels.size(); i++)
    {
        if (InviteChannels[i] == name)
        {
            InviteChannels.erase(InviteChannels.begin() + i);
            return ;
        }
    }
}

void Client::addChannel(Channel* channel) {
    _channels.push_back(channel);
}

void Client::addInviteChannel(std::string &name) {
    InviteChannels.push_back(name);
}
