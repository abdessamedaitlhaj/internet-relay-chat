#pragma once

#define CRLF "\r\n"

#define RPL_WELCOME(username, hostname) "001 :Welcome to the Internet Relay Network " + username + "!" + hostname + CRLF
#define ERR_NOTREGISTERED(nickname) "451 " + nickname + " :You have not registered" + CRLF
#define ERR_NOSUCHCHANNEL(channel) "403 " + channel + " :No such channel" + CRLF
#define ERR_UNKNOWNCOMMAND(command) "421 " + command + " :Unknown command" + CRLF
#define ERR_NEEDMOREPARAMS(command) "461 " + command + " :Not enough parameters" + CRLF
#define ERR_ALREADYREGISTERED(nickname) "462 " + nickname + " :You may not reregister\r\n"
#define ERR_PASSMISMATCH(nickname) "464 " + nickname + " :Password incorrect" + CRLF
#define ERR_NICKNAMEINUSE(nickname) "433 " + nickname + " :Nickname is already in use" + CRLF
#define ERR_ERRONEUSNICKNAME(nickname) "432 " + nickname + " :Erroneous nickname" + CRLF
#define ERR_NOTONCHANNEL(channelName) "442 " + channelName + " :You're not on that channel" + CRLF
#define RPL_NOTOPIC(channelName) "331 " + channelName + " :No topic is set" + CRLF
#define RPL_TOPIC(channelName, topic) "332 " + channelName + " :" + topic + CRLF
