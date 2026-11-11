NAME =		scop
CXX =		c++
CC =		gcc

SDIR =		./src/
SFILES =	main.cpp vec3.cpp vec2.cpp mat4.cpp camera.cpp object.cpp \
			obj_model.cpp bmp_image.cpp mtl_material.cpp shader.cpp gl_errors.cpp gl_functions.cpp
SRCS =		$(addprefix ${SDIR}, ${SFILES})

ODIR =		./obj/
OFILES =	$(SFILES:.cpp=.o)
OBJS =		$(addprefix ${ODIR}, ${OFILES})

IDIR =		./inc/

CPPFLAGS =	-I $(IDIR)
CXXFLAGS =	-Wall -Wextra -Werror -std=c++11 -DGLFW_INCLUDE_NONE # -g -O0	# uncomment for debugging
CFLAGS =	#-fPIE
LDLIBS =	-lglfw -lGL

# reset text colour to default
RESET =		\e[0m

# text colours
RED =		\e[1;31m
GREEN =		\e[1;32m
YELLOW =	\e[1;33m
BLUE =		\e[1;34m
MAGENTA =	\e[1;35m
CYAN =		\e[1;36m
WHITE =		\e[1;37m

# bold text colours
B_RED =		\e[1;31m
B_GREEN =	\e[1;32m
B_YELLOW =	\e[1;33m
B_BLUE =	\e[1;34m
B_MAGENTA =	\e[1;35m
B_CYAN =	\e[1;36m
B_WHITE =	\e[1;37m

all:	$(NAME)

$(NAME):	$(ODIR) $(OBJS)
	@printf "$(CYAN)- linking executable: $(NAME)$(RESET)\n"
	@$(CXX) $(OBJS) $(LDLIBS) -o $@
	@printf "$(B_GREEN)- complete!$(RESET)\n"

$(ODIR)%.o:	$(SDIR)%.cpp | $(ODIR)
	@printf "$(BLUE)- compiling object from: $<$(RESET)\n"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<

$(ODIR):
	@printf "$(MAGENTA)- creating obj directory$(RESET)\n"
	@mkdir -p $(ODIR)

clean:
	@if [ -d "$(ODIR)" ]; then \
		rm -rf $(ODIR); printf "$(B_RED)- objects and object directory removed$(RESET)\n"; \
	else \
		printf "$(YELLOW)- object directory does not exist$(RESET)\n"; \
	fi

fclean:
	@$(MAKE) --no-print-directory clean
	@if [ -f "$(NAME)" ]; then \
		rm -rf $(NAME); printf "$(B_RED)- $(NAME) removed$(RESET)\n"; \
	else \
		printf "$(YELLOW)- $(NAME) does not exist$(RESET)\n"; \
	fi

re:
	@$(MAKE) --no-print-directory fclean
	@$(MAKE) --no-print-directory all

.PHONY:	all clean fclean re
