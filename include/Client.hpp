#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <string>

class Client {
private:
    int _fd;
    bool _registered;
    std::string _nickname;
    std::string _username;
    std::string _hostname;
    std::string _buffer;

public:
    Client();
    Client(int fd);
    ~Client();

    int getFd() const;
    bool isRegistered() const;
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getHostname() const;
    std::string getBuffer() const;

    void setFd(int fd);
    void setRegistered(bool registered);
    void setNickname(const std::string& nickname);
    void setUsername(const std::string& username);
    void setHostname(const std::string& hostname);
    void setBuffer(const std::string& buffer);
    void appendToBuffer(const std::string& str);
};

#endif