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

std::string Channel::getTopic() const {
    return _topic;
}

void Channel::setTopic(const std::string &topic) {
    _topic = topic;
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
        if (_members[i].getNickname() == client->getNickname()) {
            return true;
        }
    }
    return false;
}

bool Channel::isOperator(Client *client) const {
    for (size_t i = 0; i < _operators.size(); ++i) {
        if (&_operators[i] == client) {
            return true;
        }
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

// void Channel::sendToClient(Client *client, const std::string &message) const {
//     // Send message to client
// }

