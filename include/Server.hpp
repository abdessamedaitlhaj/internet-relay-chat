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
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <map>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

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
		struct sockaddr_in _serverAddress;
		struct pollfd _poll;
		std::string _password;
		std::map<int, Client> _clients;
		std::vector<struct pollfd> _pollFds;
		std::vector<Channel*> _channels;
		std::string _serverName;

	public:
		Server(char** av);
		bool accept_cl();
		void receive(size_t & i);
		void						setup();
		void						serverSocket();
		int							parse_port(std::string port);
		std::string 				parse_password(std::string password);
		static void 				breakSignal(int signum);
		Client*						getClient(int fd);

		Channel 					*getChannel(std::string &name);
		std::vector<std::string> 	parseData(Client* client);
		void 						parseCommand(int fd, std::string input);
		bool 						isNickNameInUse(const std::string& nickname);
		void 						sendResponse(int fd, const std::string& response);
		void 						handleBuffer(int fd, std::string &buffer);
		std::vector<std::string> 	split(const std::string &str, const std::string &delimiters);
		
		// AUTH
		void						handlePass(int fd, std::string &input, Client &client);
		void						handleNick(int fd, std::string &input, Client &client);
		void						handleUser(int fd, std::string &input, Client &client);
		void						handleTopic(int fd, std::string &input, Client &client);
		void						handlePrivmsg(int fd, std::string &input, Client& client);
		void						handleJoin(int fd, std::string &input, Client& client);
		void						handleMode(int fd, std::string &input, Client &client);
		void						handlePart(int fd, std::string &input, Client &client);
		bool						channelNameValid(std::string &channelName);
		void 						addChannel(Channel *channel);
		std::string 				getMsg(std::vector<std::string> &tokens);
		

		Client						*getClientNick(std::string &nick);
		Client						*getClientUserName(std::string &nick);
};