#pragma once 

#include "Client.hpp"
#include "Channel.hpp"
#include "numericReplies.hpp"

#include <string>
#include <poll.h>
#include <fcntl.h>
#include <signal.h>
#include <cctype>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include <ctime>
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

#define RESET   "\e[0m"
#define RED     "\e[1;31m"
#define GREEN   "\e[1;32m"
#define YELLOW  "\e[1;33m"
#define MAGENTA "\e[1;35m"

struct ModeChange {
    char mode;
    char sign;
    std::string argument;
};

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

	public:
		Server(char** av);
		~Server();
		bool						accept_cl();
		void						receive(size_t & i);
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
		void						handlePass(int fd, std::string &input, Client &client);
		void						handleNick(int fd, std::string &input, Client &client);
		void						handleUser(int fd, std::string &input, Client &client);
		void						handleTopic(int fd, std::string &input, Client &client);
		void						handlePrivmsg(int fd, std::string &input, Client& client);
		void						handleJoin(int fd, std::string &input, Client& client);
		void						handleMode(int fd, std::string &input, Client &client);
		void						handleInvite(int fd, std::string &input, Client &client);
		void						handleKick(int fd, std::string &input, Client &client);
		void						handlePart(int fd, std::string &input, Client &client);
		void						handleQuit(int fd, std::string &input, Client &client);
		bool						channelNameValid(std::string &channelName);
		void 						addChannel(Channel *channel);
		std::string 				checkModes(int fd, ModeChange &modeChange, Channel &channel, std::map<char, std::string> &params);
		Client						*getClientNick(std::string &nick);
		Client						*getClientUserName(std::string &nick);
		void						removeFd(int fd);
		void						removeChannel(Client &client);
		void						removeClient(int fd);
		std::string					getAppliedModes(std::vector<ModeChange>& modeChanges, Channel &channel);
		std::string					applyModes(int fd, std::vector<ModeChange>& modeChanges, Channel &channel);
		std::string					getTrailing(std::vector<std::string> &tokens, std::string &trailing, std::string &input);
		bool						isChannel(int fd, std::string &target, Client &client, std::string &trailing);
		bool						isClient(int fd, std::string &target, Client &client, std::string &trailing);

};