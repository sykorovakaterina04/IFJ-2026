CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.DEFAULT_GOAL := all
.PHONY all run build check build-check test clean

TARGET = IFJ6
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))


all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

#check:

#build-check: check build

#test:

clean:
	rm -rf $(OBJ_DIR) $(TARGET)