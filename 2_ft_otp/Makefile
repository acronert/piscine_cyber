NAME		=	ft_otp
CC			=	cc
FLAGS		=	-Wall -Wextra -Werror -g3

SRCS		=	./srcs/main.c	\
				./srcs/otp.c	\
				./srcs/crypt.c

HEADERS		=	-I ./include/
LIBS		=	-lm -lcrypto

OBJS		=	$(SRCS:.c=.o)

all:	$(NAME)

$(NAME): $(OBJS)
	@$(CC) $(FLAGS) $(OBJS) -o $(NAME) $(HEADERS) $(LIBS) 
	@echo "Compiling $(NAME)"

%.o: %.c
	@$(CC) $(FLAGS) $(HEADERS)  -c  $< -o $@
	@echo "Compiling $<"

clean:
	@rm -rf $(OBJS)
	@echo "Cleaning..."

fclean: clean
	@rm -rf $(NAME)

re: fclean all

.PHONY : all clean fclean re
