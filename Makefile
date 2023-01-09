# C 			= gcc
# LD			= gcc
# CFLAG		= -Wall
# PROG_NAME   = main
# SRC_DIR		=./src
# INC_DIR		=./inc
# OBJ_DIR		=./obj
# BIN_DIR		=./bin
# SRC_LIST = $(wildcard $(SRC_DIR)/*.c)
# OBJ_LIST = $(wildcard $(OBJ_DIR)/*.o)

# .PHONY: all clean $(PROG_NAME) complie

# all: $(PROG_NAME)

# complie:
# 	$(CC)	$(CFLAG)	$(SRC_LIST)	-I	$(INC_DIR)	-c
# copy:	complie
# 	@mv	$(notdir	$(SRC_LIST:.c=.o))	$(OBJ_DIR)/

# $(PROG_NAME):	copy
# 	$(LD)	$(OBJ_LIST)	-o	$(BIN_DIR)/$@

# clean:
# 	rm	-f	$(BIN_DIR)/$(PROG_NAME)	$(OBJ_DIR)/*.o

# LDFLAGS = -pthread -lpthread
# CFLAGS = -g -Wall -Werror
# backprop: backprop.o boardbuilding.o printboard.o
# 	$(CC) $(LDFLAGS) -o backprop main.o boardbuilding.o printboard.o -lm

# backprop.o: main.c
# 	$(CC) $(CFLAGS) -c main.c

# boardbuilding.o: boardbuilding.c
# 	$(CC) $(CFLAGS) -c boardbuilding.c

# printboard.o: printboard.c
# 	$(CC) $(CFLAGS) -c printboard.c

# # remove object files and executable when user executes "make clean"
# clean:
# 	rm *.o backprop