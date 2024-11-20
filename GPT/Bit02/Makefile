# Variables
CC = gcc
CFLAGS = -Wall -g
LIBS = -lSDL2 -lGLEW -lGL
SRC = main.c
TARGET = brickout

# Default target
all: $(TARGET)

# Compile the project
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean the build files
clean:
	rm -f $(TARGET)

# Run the project
run: $(TARGET)
	./$(TARGET)
