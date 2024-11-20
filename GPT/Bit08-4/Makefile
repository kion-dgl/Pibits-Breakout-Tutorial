# Variables
CC = gcc
CFLAGS = -Wall -g
# SDL2, SDL_image, and OpenGL libraries
LIBS = -lSDL2 -lSDL2_image -lGL -lGLEW -lm

# Source files
SRCS = main.c utils.c init.c

# Executable output
OUT = brickout

# Build rules
all: $(OUT)

$(OUT): $(SRCS)
	$(CC) $(CFLAGS) -o $(OUT) $(SRCS) $(LIBS)

# Clean up build files
clean:
	rm -f $(OUT)
