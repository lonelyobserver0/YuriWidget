CC = gcc
CFLAGS = -Wall -g -Iinclude `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0 json-glib-1.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0 json-glib-1.0` -pthread
SRC = src/main.c src/config.c src/window.c src/server.c
OBJ = $(SRC:.c=.o)
TARGET = yuriwidget

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
