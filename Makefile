CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -fsanitize=address -g
SRC = main.cpp	src/Client.cpp src/Server.cpp src/auth.cpp src/Channel.cpp src/cmds/topic.cpp src/cmds/join.cpp src/server_utils.cpp src/cmds/privmsg.cpp src/cmds/mode.cpp src/cmds/invite.cpp src/cmds/kick.cpp src/cmds/quit.cpp
OBJ = $(SRC:.cpp=.o)
HEADER = include/Server.hpp include/Client.hpp include/Channel.hpp include/numericReplies.hpp
NAME = ircserv
NAME_BONUS = bot

BOT = bonus/bot.cpp  bonus/main.cpp
QUESTION =  bonus/question.cpp 
BOT_HEADER = bonus/bot.hpp bonus/pool.hpp bonus/question.hpp
POOL_HEADER = bonus/pool.hpp bonus/question.hpp
BOT_BONUS = $(BOT:.cpp=.o)
QUESTION_BONUS = $(QUESTION:.cpp=.o)


all: $(NAME)
bonus: $(NAME_BONUS)

$(NAME): $(OBJ)
	c++ $(CPPFLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp $(HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 

$(NAME_BONUS): $(BOT_BONUS) $(QUESTION_BONUS)
	c++ $(CPPFLAGS) $(BOT_BONUS) $(QUESTION_BONUS)  -o $(NAME_BONUS) 

$(BOT_BONUS): %.o: %.cpp $(BOT_HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 

$(QUESTION_BONUS): %.o: %.cpp $(POOL_HEADER)
	c++ $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -f $(OBJ) $(QUESTION_BONUS)  $(BOT_BONUS)

fclean: clean
	rm -f $(NAME) $(NAME_BONUS) $(QUESTION_BONUS)  $(BOT_BONUS)

re: fclean all

.PHONY: all clean fclean re 