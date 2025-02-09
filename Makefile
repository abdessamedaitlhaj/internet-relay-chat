CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS += -fsanitize=address
SRC = main.cpp	src/Client.cpp src/Server.cpp src/auth.cpp src/Channel.cpp src/cmds/topic.cpp src/cmds/join.cpp src/server_utils.cpp src/cmds/privmsg.cpp src/cmds/mode.cpp 
OBJ = $(SRC:.cpp=.o)
HEADER = include/Server.hpp include/Client.hpp include/Channel.hpp include/numericReplies.hpp
NAME = ircserv


BOT = bot/bot.cpp bot/question.cpp 
BOT_HEADER = bot/bot.hpp bot/question.hpp bot/pool.hpp
OBJ_BONUS = $(BOT:.cpp=.o)


all: $(NAME)

$(NAME): $(OBJ) $(OBJ_BONUS)
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)


%.o: %.cpp $(HEADER) $(BOT_HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 


clean:
	rm -f $(OBJ) $(OBJ_BONUS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re 