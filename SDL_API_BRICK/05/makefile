CC = gcc
CFLAGS = -Wall -Wextra -O2 -Wno-unused-parameter `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

SRC = main.c
OBJ = $(SRC:.c=.o)

TARGET = triangle_app
INSTALL_DIR = /home/pi/RetroPie/roms/ports
SCRIPT_NAME = SDL_API_Brick.sh

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

install: $(TARGET)
	mkdir -p $(INSTALL_DIR)
	echo "#!/bin/bash\n$(PWD)/$(TARGET)" > $(INSTALL_DIR)/$(SCRIPT_NAME)
	chmod +x $(INSTALL_DIR)/$(SCRIPT_NAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean install
