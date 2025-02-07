#pragma once
#define CRLF "\r\n"

#define ERR_NOTREGISTERED(nickName) "451 " + nickName + " :You have not registered" + CRLF
#define ERR_UNKNOWNCOMMAND(nickName, command) ": 421 " + nickName + " " + command + " :Unknown command" + CRLF
#define ERR_NEEDMOREPARAMS(nickName, command) ": 461 " + nickName + " " + command + " :Not enough parameters" + CRLF
#define ERR_ALREADYREGISTERED(nickName) "462 " + nickName + " :You may not reregister" + CRLF
#define ERR_PASSMISMATCH(nickName) "464 " + nickName + " :Password incorrect" + CRLF
#define ERR_NICKNAMEINUSE(nickName) "433 " + nickName + " :Nickname is already in use" + CRLF
#define ERR_ERRONEUSNICKNAME(nickName) "432 " + nickName + " :Erroneous nickname" + CRLF
#define ERR_NOTONCHANNEL(nickName, channelName) ": 442 " + nickName + " #" + channelName + " :You're not on that channel" + CRLF
#define ERR_NOSUCHCHANNEL(nickName, channelName) ": 403 " + nickName + " #" + channelName + " :No such channel" + CRLF
#define ERR_NOSUCHNICK(nickName, recipient) ": 401 " + nickName + " " + recipient + " :No such nick/channel" + CRLF
#define ERR_NORECIPIENT(nickName) ": 411 " + nickName + " :No recipient given (PRIVMSG)" + CRLF
#define ERR_NOTEXTTOSEND(nickName) ": 412 " + nickName + " :No text to send" + CRLF
#define RPL_WELCOME(nickName, userName, hostName, ip) ": 001 " + nickName + " :Welcome to the Internet Relay Network " + hostName + ip + CRLF
#define RPL_NOTOPIC(nickName, hostName, ip, channelName) ":@" + ip + " 331 " + nickName + " #" + channelName + " :No topic is set" + CRLF
#define RPL_TOPIC(nickName, hostName, ip, channelName, topic) ":" + hostName + ip + " " + nickName + " #" + channelName + " :" + topic + CRLF
#define ERR_INVITEONLYCHAN(nickName, channelName) ": 473 " + nickName + " #" + channelName + " :Cannot join channel (+i)" + CRLF
#define RPL_CHANNELMODEIS(nickName, channelName, mode) ": 324 " + nickName + " #" + channelName + " " + mode + CRLF
#define RPL_CREATIONTIME(nickName, channelName, time) ": 329 " + nickName + " #" + channelName + " " + time + CRLF
#define ERR_CHANOPRIVSNEEDED(nickName, channelName) ": 482 " + nickName + " #" + channelName + " :You're not channel operator" + CRLF
#define RPL_TOPICWHOTIME(nickName, channelName, who, time) ":@localhost 333 " + nickName + " #" + channelName + " " + who + " " + time + CRLF