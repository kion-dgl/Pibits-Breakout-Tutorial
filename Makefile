CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g $(shell pkg-config --cflags sdl2)
INCLUDES = -Iinclude
LIBS = $(shell pkg-config --libs sdl2) -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lm

SRCDIR = src
OBJDIR = obj
INCDIR = include

SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

TARGET = brickout

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGET)

install-deps:
	sudo apt update
	sudo apt install -y libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)