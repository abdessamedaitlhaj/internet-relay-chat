CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS += #-fsanitize=address
SRC = main.cpp	src/Client.cpp src/Server.cpp src/auth.cpp src/Channel.cpp src/cmds/topic.cpp src/cmds/join.cpp src/server_utils.cpp src/cmds/privmsg.cpp src/cmds/mode.cpp src/cmds/invite.cpp src/cmds/kick.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = include/Server.hpp include/Client.hpp include/Channel.hpp include/numericReplies.hpp
NAME = ircserv
NAME_BONUS = bot

BOT = bonus/bot.cpp bonus/question.cpp bonus/main.cpp
SRC += ${BOT}
BOT_HEADER = bonus/bot.hpp bonus/question.hpp bonus/pool.hpp
OBJ_BONUS = $(BOT:.cpp=.o)


all: $(NAME)
bonus: $(NAME_BONUS)

$(NAME): $(OBJ)
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 

$(NAME_BONUS): $(OBJ_BONUS)
	c++ $(CPPFLAGS) $(OBJ_BONUS)  -o $(NAME_BONUS) 

$(OBJ_BONUS): %.o: %.c $(BOT_HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 


clean:
	rm -f $(OBJ) $(OBJ_BONUS)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS)

re: fclean all

.PHONY: all clean fclean re 