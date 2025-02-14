#pragma once 

#include "question.hpp"
#include "pool.hpp"
#include <iostream>
#include "bot.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <sys/wait.h>

// Define colors
#define RESET   "\e[0m"
#define RED     "\e[1;31m"
#define GREEN   "\e[1;32m"
#define YELLOW  "\e[1;33m"
#define BLUE    "\e[1;34m"
#define MAGENTA "\e[1;35m"
#define CYAN    "\e[1;36m"
#define WHITE   "\e[1;37m"

#define LOCALHOST "127.0.0.1"
#define USERNAME "bot default given name"



class Bot {
	private:
		int		parse_port(std::string port);
		std::string 	parse_password(std::string password);
		std::string 	parse_nick(std::string nick);
		std::map<std::string, QuestionGame> gameMap;
	public :
		std::string extractNickname(std::string& message);
		bool _log;
		std::string _password;
		std::string _nick;
		int _port;
		int botsock;
		Bot(char **av);
		~Bot();
		void setup();
		void sendResponse(int fd, const std::string& response);
		void game();
		void play(QuestionGame& game, std::string& UserNick); 

};