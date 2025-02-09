#include "../include/Channel.hpp"
                        

Channel::Channel(const std::string &name) : _name(name) {
    _inviteOnly = false;
    _topicRestriction = false;
    _userLimit = false;
}

Channel::~Channel() {
}

std::string Channel::getName() const {
    return _name;
}

bool Channel::getlimit() const {
    return _userLimit;
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
bool Channel::getTopicRestriction() const {
    return _topicRestriction;
}

time_t Channel::getTopicTime() const {
    return _topicTime;
}
int Channel::getclientsnumber() const {
    return _members.size(); //check operator
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
        if (&_members[i] == client) {
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
        if (&_operators[i] == client) {
            _operators.erase(_operators.begin() + i);
            return;
        }
    }
}

bool Channel::isMember(Client *client) const {
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
bool Channel::isInvited(Client *client, std::string name, int flg) const {
    if (client->getInviteChannel(name)){
        if (flg == 1){
            client->removeChannelInvite(name);
        }
        return true;
    }
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
        if (&_members[i] != sender) {
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
        // Avoid duplicate entries
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
