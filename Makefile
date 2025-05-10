
APPNAME = yuriwidget
CC = cc
SRC = yuriwidget.c
OBJ = $(SRC:.c=.o)
DEPS_DIR = deps/tomlc99
PKG_MODULES = gtk4 gtk-layer-shell-0
PKG_CFLAGS = $(shell pkg-config --cflags $(PKG_MODULES))
PKG_LIBS = $(shell pkg-config --libs $(PKG_MODULES))

all: $(APPNAME)

$(APPNAME): $(OBJ)
	$(CC) -o $@ $^ -I$(DEPS_DIR) $(PKG_CFLAGS) $(PKG_LIBS)

%.o: %.c
	$(CC) -c $< -I$(DEPS_DIR) $(PKG_CFLAGS)

deps:
	git submodule update --init --depth 1 --recommend-shallow deps/tomlc99

clean:
	rm -f $(APPNAME) $(OBJ)

.PHONY: all clean deps
