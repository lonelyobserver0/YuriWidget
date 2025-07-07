CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0 json-glib-1.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0 json-glib-1.0` -pthread

INCLUDES = -Iinclude -Isrc/server/headers

CLIENT_SRC = src/client/yuriwidget_client.c
CLIENT_BIN = build/yuriwidgetctl

SERVER_SRC = \
    src/server/main.c \
    src/server/window.c \
    src/server/config.c \
    src/server/socket_server.c

SERVER_BIN = build/yuriwidget

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)


PREFIX ?= /usr/local

install: all
	install -d $(PREFIX)/bin
	install -m 755 $(CLIENT_BIN) $(PREFIX)/bin/
	install -m 755 $(SERVER_BIN) $(PREFIX)/bin/
