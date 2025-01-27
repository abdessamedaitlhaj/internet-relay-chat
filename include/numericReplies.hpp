#pragma once

#define CRLF "\r\n"

#define RPL_WELCOME(nickname, username, hostname) "001 " + nickname + " :Welcome to the Internet Relay Network " + username + "!" + hostname + CRLF
#define ERR_NOTREGISTERED(nickname) "451 " + nickname + " :You have not registered" + CRLF
#define ERR_NOSUCHNICK(nickname) "401 " + nickname + " :No such nick/channel" + CRLF
#define ERR_NOSUCHCHANNEL(nickname, channel) "403 " + nickname + " " + channel + " :No such channel" + CRLF
#define ERR_CANNOTSENDTOCHAN(nickname, channel) "404 " + nickname + " " + channel + " :Cannot send to channel" + CRLF
#define ERR_NORECIPIENT(nickname) "411 " + nickname + " :No recipient given" + CRLF
#define ERR_NOTEXTTOSEND(nickname) "412 " + nickname + " :No text to send" + CRLF
#define ERR_UNKNOWNCOMMAND(nickname, command) "421 " + nickname + " " + command + " :Unknown command" + CRLF
#define ERR_NEEDMOREPARAMS(command) "461 " + command + " :Not enough parameters" + CRLF
#define ERR_ALREADYREGISTERED(nickname) "462 " + nickname + " :You may not reregister\r\n"
#define ERR_PASSMISMATCH(nickname) "464 " + nickname + " :Password incorrect" + CRLF
#define ERR_NICKNAMEINUSE(nickname) "433 " + nickname + " :Nickname is already in use" + CRLF
#define ERR_ERRONEUSNICKNAME(nickname) "432 " + nickname + " :Erroneous nickname" + CRLF