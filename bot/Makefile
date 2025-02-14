CPPFLAGS = -Wall -Wextra -Werror -std=c++98
CPPFLAGS += #-fsanitize=address
SRC = bot.cpp main.cpp question.cpp 
OBJ = $(SRC:.cpp=.o)
HEADER = bot.hpp pool.hpp question.hpp
NAME = bot



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