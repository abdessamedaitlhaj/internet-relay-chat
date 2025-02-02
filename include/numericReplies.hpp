#pragma once
#define CRLF "\r\n"

#define RPL_WELCOME(nickName, userName, hostName) "001 :Welcome to the Internet Relay Network " + nickName + "!" + userName + "@" + hostName + CRLF
#define ERR_NOTREGISTERED(nickName) "451 " + nickName + " :You have not registered" + CRLF
#define ERR_NOSUCHCHANNEL(channelName) "403 " + channelName + " :No such channel" + CRLF
#define ERR_UNKNOWNCOMMAND(command) "421 " + command + " :Unknown command" + CRLF
#define ERR_NEEDMOREPARAMS(command) "461 " + command + " :Not enough parameters" + CRLF
#define ERR_ALREADYREGISTERED(nickName) "462 " + nickName + " :You may not reregister\r\n"
#define ERR_PASSMISMATCH(nickName) "464 " + nickName + " :Password incorrect" + CRLF
#define ERR_NICKNAMEINUSE(nickName) "433 " + nickName + " :Nickname is already in use" + CRLF
#define ERR_ERRONEUSNICKNAME(nickName) "432 " + nickName + " :Erroneous nickname" + CRLF
#define ERR_NOTONCHANNEL(channelName) "442 " + channelName + " :You're not on that channel" + CRLF
#define RPL_NOTOPIC(channelName) "331 " + channelName + " :No topic is set" + CRLF
#define RPL_TOPIC(channelName, topic) "332 " + channelName + " : " + topic + CRLF
#define ERR_NOSUCHNICK(nickName) "401 " + nickName + " :No such nick" + CRLF
