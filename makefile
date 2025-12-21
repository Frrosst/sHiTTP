CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Ilib
SRC_DIR = src
LIB_DIR = lib
OBJ_DIR = build
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c $(LIB_DIR)/parser.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/parser.o
TARGET = $(BIN_DIR)/main

all: create_dirs $(TARGET)

create_dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Explicit dependencies
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(LIB_DIR)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/parser.o: $(LIB_DIR)/parser.c $(LIB_DIR)/parser.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run create_dirs