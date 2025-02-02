#pragma once 

#include "Client.hpp"
#include "Channel.hpp"
#include "numericReplies.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <poll.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <algorithm>

// Define colors
#define RESET   "\e[0m"
#define RED     "\e[1;31m"
#define GREEN   "\e[1;32m"
#define YELLOW  "\e[1;33m"
#define BLUE    "\e[1;34m"
#define MAGENTA "\e[1;35m"
#define CYAN    "\e[1;36m"
#define WHITE   "\e[1;37m"

class Client;

class Server {

	private:
		int _port;
		int _socket;
		static int _break;
		struct sockaddr_in serverAddress;
		struct pollfd _poll;
		std::string _password;
		std::map<int, Client> _clients;
		std::vector<struct pollfd> _pollfds;
		std::vector<Channel*> _channels;

	public:
		Server(char** av);
		void						setup();
		void						serverSocket();
		int							parse_port(std::string port);
		std::string 				parse_password(std::string password);
		static void 				breakSignal(int signum);
		Client*						getClient(int fd);

		Channel 					*getChannel(std::string &name);
		std::vector<std::string> 	parseData(Client* client);
		void 						parseCommand(int fd, std::string input);
		bool 						isNicknameInUse(const std::string& nickname);
		void 						sendResponse(int fd, const std::string& response);
		void 						handleBuffer(int fd, std::string &buffer);
		std::vector<std::string> 	split(const std::string& str, const std::string& delimiters);
		
		// handle commands
		void						pass(std::vector<std::string> &tokens, Client &client);
		void						nick(std::vector<std::string> &tokens, Client &client);
		void						user(std::vector<std::string> &tokens, std::string &trailing, Client &client);
		void						topic(std::vector<std::string> tokens, std::string &trailing, Client &client);
		void    					privmsg(std::vector<std::string> &tokens, std::string &trailing, Client &client);

		bool						channelNameValid(std::string &channelName);
		void 						addChannel(Channel *channel);
		Client						*getClientNick(std::string &nick);
};
