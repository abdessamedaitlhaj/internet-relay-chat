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

std::string Client::getNickname() const {
    return _nickname;
}

std::string Client::getUsername() const {
    return _username;
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

void Client::setNickname(const std::string& nickname) {
    _nickname = nickname;
}

void Client::setUsername(const std::string& username) {
    _username = username;
}

void Client::setHostname(const std::string& hostname) {
    _hostname = hostname;
}

void Client::setBuffer(const std::string& buffer) {
    _buffer = buffer;
}

void Client::appendToBuffer(const std::string& str) {
    _buffer += str;
}