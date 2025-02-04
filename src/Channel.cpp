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
bool Channel::getInviteOnly() const {
    return _inviteOnly;
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
bool Channel::isInvited(Client *client, std::string name, int flg) const {
    if (client->getinvitechannel(name)){
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

// send messages

// void Channel::broadcast(const std::string &message, Client *sender) const {
//     for (size_t i = 0; i < _members.size(); ++i) {
//         if (&_members[i] != sender) {
//             sendToClient(&_members[i], message);
//         }
//     }
// }

// void Channel::sendToClient(Client *client, const std::string &message) const {
//     // Send message to client
// }

