# Compiler and flags
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -g
LDFLAGS = -lws2_32

# Executable name
TARGET = build/udsp_game_of_life_server

# Source files and headers
SRCS = main.c server.c
HEADERS = server.h

# Default target
all: build $(TARGET)

# Create build directory
build:
	mkdir -p build

# Compile the target
$(TARGET): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LDFLAGS)

# Clean build artifacts
clean:
	rm -rf build

# Phony targets
.PHONY: all clean
