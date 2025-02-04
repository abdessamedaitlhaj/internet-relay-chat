#include "../include/Client.hpp"

// Constructor
Client::Client() : _fd(-1), _registered(false), _nickname(""), _username(""), _hostname(""), _buffer("") {}

Client::Client(int fd) : _fd(fd), _registered(false), _nickname(""), _username(""), _hostname(""), _buffer("") {}

// Destructor
Client::~Client() {}

// Getter methods
int Client::getFd() const {
    return _fd;
}

bool Client::isRegistered() const {
    return _registered;
}

bool Client::getinvitechannel(const std::string& name) const
{
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->getName() == name)
            return true;
    }
    return false;
}

std::string Client::getPassword() const {
    return _password;
}

std::string Client::getNickname() const {
    return _nickname;
}

std::string Client::getUsername() const {
    return _username;
}

std::string Client::getRealname() const {
    return _relaname;
}

std::string Client::getHostname() const {
    return _hostname;
}

std::string Client::getBuffer() const {
    return _buffer;
}

// Setter methods
void Client::setFd(int fd) {
    _fd = fd;
}

void Client::setRegistered(bool registered) {
    _registered = registered;
}

void Client::setPassword(const std::string& password) {
    _password = password;
}

void Client::setNickName(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUserName(const std::string& username) {
    _username = username;
}

void Client::setRealName(const std::string& realname) {
    _relaname = realname;
}

void Client::setHostName(const std::string& hostname) {
    _hostname = hostname;
}

void Client::setBuffer(const std::string& buffer) {
    _buffer += buffer;
}

void Client::clearBuffer() {

    _buffer.clear();
}

void Client::removeChannelInvite(std::string &name) {
    for (size_t i = 0; i < _channels.size(); i++)
    {
        if (_channels[i]->getName() == name)
        {
            _channels.erase(_channels.begin() + i);
            return ;
        }
    }
}

void Client::addChannel(Channel* channel) {
    _channels.push_back(channel);
}
