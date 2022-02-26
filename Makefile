# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: acortes- <acortes-@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/06 18:21:39 by mrosario          #+#    #+#              #
#    Updated: 2022/02/26 20:04:21 by acortes-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCDIR = ./srcs

GIT_VERSION := "$(shell git describe --abbrev=4 --dirty --always --tags)"

GRN = \e[1;32m
RED = \e[1;31m
YEL = \e[1;33m
DEL = \e[2K\r
RST = \e[0m

CXX = clang++

SRCFILES =	./main.cpp $(SRCDIR)/ircserv.cpp \
			$(SRCDIR)/ircserv_interpreters.cpp \
			$(SRCDIR)/ircserv_numeric_replies.cpp \
			$(SRCDIR)/ircserv_numeric_error_replies.cpp \
			$(SRCDIR)/ircserv_other_replies.cpp \
			$(SRCDIR)/client.cpp \
			$(SRCDIR)/channel.cpp

OBJS = $(SRCFILES:.cpp=.o)

INCLUDES = -I $(SRCDIR)

CXXFLAGS = -Wall -Werror -Wextra -g -std=c++98 -DVERSION=\"$(GIT_VERSION)\"

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -ltermcap -o $(NAME) $(MAIN_O) $(OBJS)

clean:
	@printf "$(YEL)Deleting object files...$(RST)"
	@rm $(OBJS) 2>/dev/null || true
	@printf "$(DEL)$(GRN)Deleted object files\n$(RST)"

fclean: clean
	@printf "$(YEL)Deleting program...$(RST)"
	@rm $(NAME) 2>/dev/null || true
	@printf "$(DEL)$(GRN)Deleted program\n$(RST)"

re: fclean all

.PHONY: all clean fclean re
