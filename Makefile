NAME =		scop
CXX =		g++

SDIR =		./src/
SFILES =	main.cpp vec3.cpp vec2.cpp mat4.cpp camera.cpp object.cpp obj_model.cpp bmp_image.cpp mtl_material.cpp shader.cpp
SRCS =		$(addprefix ${SDIR}, ${SFILES})

GLAD =		./src/glad.c

# GLFW =		./glfw/

ODIR =		./obj/
OFILES =	$(SFILES:.cpp=.o)
OBJS =		$(addprefix ${ODIR}, ${OFILES})

OBJSGLAD = $(ODIR)glad.o

IDIR =		./inc/

#DEPSDIR =	./deps

#GLFW_PREFIX =	$(DEPSDIR)/glfw
#GLFW_SRC =	$(DEPSDIR)/glfw-src
#GLFW_BUILD =	$(DEPSDIR)/glfw-build
#GLFW_LIB =	$(GLFW_PREFIX)/lib/libglfw3.a
#GLFW_INC =	$(GLFW_PREFIX)/include

CXXFLAGS =	-I $(IDIR) -Wall -Wextra -Werror -std=c++11 -g -O1

LDLIBS =	-lglfw -lGL -ldl -lpthread -lm -lX11 -lXrandr -lXi -lXxf86vm -lwayland-client -lwayland-cursor

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

$(NAME):	$(ODIR) $(OBJS) $(OBJSGLAD) #$(GLFW_LIB) #$(GLEW_LIB)
	@printf "$(CYAN)- linking executable: $(NAME)$(RESET)\n"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(OBJS) $(OBJSGLAD) $(LDLIBS) -o $@
	@printf "$(B_GREEN)- complete!$(RESET)\n"

# $(GLFW_LIB):
# 	@printf "$(MAGENTA)- preparing GLFW in $(GLFW_PREFIX)$(RESET)\n"
# 	@mkdir -p $(DEPSDIR)
# 	@if [ ! -d "$(GLFW_SRC)" ]; then \
# 		echo "Cloning GLFW..."; \
# 		git clone --depth 1 https://github.com/glfw/glfw.git $(GLFW_SRC); \
# 	fi
# 	@rm -rf $(GLFW_BUILD)
# 	mkdir -p $(GLFW_BUILD)
# 	cmake -S $(GLFW_SRC) -B $(GLFW_BUILD) \
# 		-DCMAKE_BUILD_TYPE=Release \
# 		-DBUILD_SHARED_LIBS=OFF \
# 		-DFWGLW_BUILD_EXAMPLES=OFF \
# 		-DGLFW_BUILD_EXAMPLES=OFF \
# 		-DGLFW_BUILD_TESTS=OFF \
# 		-DGLFW_BUILD_DOCS=OFF \
# 		-DCMAKE_INSTALL_PREFIX=$(GLFW_PREFIX)
# 	@cmake --build $(GLFW_BUILD) -j
# 	@cmake --install $(GLFW_BUILD)

$(ODIR)glad.o: $(GLAD) | $(ODIR)
	gcc $(CPPFLAGS) -I$(IDIR) -fPIE -c $< -o $@

$(ODIR)%.o:	$(SDIR)%.cpp
	@printf "$(BLUE)- compiling object from: $<$(RESET)\n"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ -c $<
#	@printf "$(BLUE)- compiling object from: \n"
#	@$(CXX) $(CPPFLAGS) -o $@ -c $(SDIR)/glad.c

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
