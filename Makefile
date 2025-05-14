APPNAME = yuriwidget
CC = cc
SRC = yuriwidget.c deps/tomlc99/toml.c
OBJ = $(SRC:.c=.o)
DEPS_DIR = deps/tomlc99
PKG_MODULES = gtk4 gtk-layer-shell-0
PKG_CFLAGS = $(shell pkg-config --cflags $(PKG_MODULES))
PKG_LIBS = $(shell pkg-config --libs $(PKG_MODULES))
PREFIX = /usr/local
SYSCONFDIR = /etc/yuriwidget
CONFIG_SRC = config/config.toml
CONFIG_DEST = $(DESTDIR)$(SYSCONFDIR)/config.toml

all: $(APPNAME)

$(APPNAME): $(OBJ)
	$(CC) -o $@ $^ $(PKG_CFLAGS) $(PKG_LIBS)

%.o: %.c
	$(CC) -c $< -I$(DEPS_DIR) $(PKG_CFLAGS)

deps:
	git submodule update --init --depth 1 --recommend-shallow $(DEPS_DIR)

install: $(APPNAME)
	install -Dm755 $(APPNAME) $(DESTDIR)$(PREFIX)/bin/$(APPNAME)
	@if [ ! -f $(CONFIG_DEST) ]; then \
		echo "Installing default config to $(CONFIG_DEST)"; \
		install -Dm644 $(CONFIG_SRC) $(CONFIG_DEST); \
	else \
		echo "Config already exists at $(CONFIG_DEST), skipping."; \
	fi

clean:
	rm -f $(APPNAME) $(OBJ)

.PHONY: all clean deps install
