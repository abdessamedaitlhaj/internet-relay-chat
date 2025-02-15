#include "../include/Channel.hpp"
                        
Channel::Channel(const std::string &name) : _name(name) {
    _inviteOnly = false;
    _topicRestriction = false;
    _userLimit = false;
    _limit = 0;
    _auth = false;
    _topicTime = 0;
    _password = "";
}

Channel::~Channel() {
}

std::string Channel::getName() const {
    return _name;
}

int Channel::getlimit() const {
    return _limit;
}

std::string Channel::getTopic() const {
    return _topic;
}
bool Channel::getInviteOnly() const {
    return _inviteOnly;
}

std::string Channel::getPassword() const {
    return _password;
}

bool Channel::getUserLimit() const {
    return _userLimit;
}

bool Channel::getTopicRestriction() const {
    return _topicRestriction;
}

time_t Channel::getTopicTime() const {
    return _topicTime;
}
int Channel::getclientsnumber() const {
    return _members.size();
}

bool Channel::getAuth() const {
    return _auth;
}

int Channel::getLimit() const {
    return _limit;
}

void Channel::setUserLimit(bool userLimit) {
    _userLimit = userLimit;
}

void Channel::setInviteOnly(bool inviteOnly) {
    _inviteOnly = inviteOnly;
}

void Channel::setLimit(int limit) {
    _limit = limit;
}

void Channel::setAuth(bool auth) {
    _auth = auth;
}

void Channel::setTopicRestriction(bool topicRestriction) {
    _topicRestriction = topicRestriction;
}

void Channel::setname(const std::string &name) {
    _name = name;
}

void Channel::setTopic(const std::string &topic) {
    _topic = topic;
    _topicTime = time(NULL);
}

void Channel::setPassword(const std::string &password) {
    _password = password;
}

void Channel::addMember(Client *client) {
    _members.push_back(*client);
}

void Channel::removeMember(Client *client) {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i].getNickName() == client->getNickName()) {
            _members.erase(_members.begin() + i);
            return;
        }
    }
}

void Channel::addOperator(Client *client) {
    _operators.push_back(*client);
}

void Channel::removeOperator(Client *client) {
    for (size_t i = 0; i < _operators.size(); ++i) {
        if (_operators[i].getNickName() == client->getNickName()) {
            _operators.erase(_operators.begin() + i);
            return;
        }
    }
}

bool Channel::isMember(Client *client) {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i].getNickName() == client->getNickName()) {
            return true;
        }
    }
    return false;
}

bool Channel::isOperator(Client *client) const {
    for (size_t i = 0; i < _operators.size(); ++i) {
        if (_operators[i].getNickName() == client->getNickName()) {
            return true;
        }
    }
    return false;
}

bool Channel::isInvited(Client *client, std::string name) const {
    if (client->getInviteChannel(name))
        return true;
    return false;
}

void Channel::setMode(char mode, bool enable) {
    _modes[mode] = enable;
}

bool Channel::hasMode(char mode) const {
    return _modes.find(mode) != _modes.end();
}

void Channel::broadcast(const std::string &message, Client *sender) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i].getNickName() != sender->getNickName()) {
            if (send(_members[i].getFd(), message.c_str(), message.length(), 0) == -1) {
                std::cerr << "Error: Failed to send message to client " << _members[i].getFd() << std::endl;
            }
        }
    }
}

std::string Channel::ChannelsclientList() {
    std::string list;
    std::set<std::string> seen;
    for (size_t i = 0; i < _members.size(); ++i) {
        std::string nickname = _members[i].getNickName();
        if (seen.find(nickname) != seen.end())
            continue;
        seen.insert(nickname);
        if (!list.empty()) 
            list += " ";
        if (isOperator(&_members[i])) {  
            list += "@" + nickname;
        } else {
            list += nickname;
        }
    }
    return list;
}

void Channel::broadcastToAll(const std::string &message) const {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (send(_members[i].getFd(), message.c_str(), message.length(), 0) == -1) {
            std::cerr << "Error: Failed to send message to client " << _members[i].getFd() << std::endl;
        }
    }
}

Client *Channel::getclient(std::string name) {
    for (size_t i = 0; i < _members.size(); ++i) {
        if (_members[i].getNickName() == name) {
            return &_members[i];
        }
    }
    return NULL;
}