CPPFLAGS = #-Wall -Wextra -Werror -std=c++98
CPPFLAGS += -fsanitize=address
SRC = main.cpp	src/Client.cpp src/Server.cpp src/auth.cpp src/Channel.cpp src/cmds/topic.cpp src/cmds/join.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = include/Server.hpp include/Client.hpp include/Channel.hpp include/numericReplies.hpp
NAME = ircserv

all: $(NAME)

$(NAME): $(OBJ) 
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 