NAME := webserver
CXX := c++
CXXFLAGS := -Wextra -Wall -Werror -std=c++98 -g
INLCUDES := -I ./includes
SRC_DIR := ./src
OBJ_DIR := ./obj

# Recursive wildcard function to find all .c files in subdirectories
rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

SRCS := $(call rwildcard,$(SRC_DIR)/,*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
OBJ_DIRS := $(sort $(dir $(OBJS)))

all: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) $< -c -o $@ $(INCLUDES)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -g -o $(NAME) $(INCLUDES)

$(OBJ_DIRS):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)



re: fclean all

.PHONY: all clean fclean re