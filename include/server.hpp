#pragma once 


#include <vector>
#include "client.hpp"



// Define colors
#define RESET   "\e[0m"
#define RED     "\e[1;31m"
#define GREEN   "\e[1;32m"
#define YELLOW  "\e[1;33m"
#define BLUE    "\e[1;34m"
#define MAGENTA "\e[1;35m"
#define CYAN    "\e[1;36m"
#define WHITE   "\e[1;37m"



class server {
	private :
		int _port;
		int	_socket;
		std::vector<client> _Clients;
	public :
		server();
};