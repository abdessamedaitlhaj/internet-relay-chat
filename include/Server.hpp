#pragma once 

#ifndef SERVER_HPP
#define SERVER_HPP


#include "../include/Client.hpp"

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

	public:
		void setup();
		void serverSocket();
		Server(char** av);
		int parse_port(std::string port);
		std::string parse_password(std::string password);
		static void breakSignal(int signum);
		Client* getClient(int fd);
		
		std::vector<std::string> parseData(Client* client);
		void parseCommand(std::string input, int fd);
		bool isNicknameInUse(const std::string& nickname);
		void sendResponse(int fd, const std::string& response);
};

#endif