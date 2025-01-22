CPPFLAGS =# -Wall -Wextra -Werror -std=c++98
NAME = ircserv
SRC = src/main.cpp	src/client.cpp src/server.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = include/server.hpp include/client.hpp include/server.hpp

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