CC = gcc
PKG_CONFIG = PKG_CONFIG_PATH=/usr/lib64/pkgconfig:$(PKG_CONFIG_PATH) pkg-config
CFLAGS = -Wall -Wextra -O2 -Ideps/tomlc99 $(shell $(PKG_CONFIG) --cflags gtk4 gtk4-layer-shell-0)
LDFLAGS = $(shell $(PKG_CONFIG) --libs gtk4 gtk4-layer-shell-0)
SRC = yuriwidget.c deps/tomlc99/toml.c
OUT = yuriwidget
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

install: $(OUT)
	install -Dm755 $(OUT) $(DESTDIR)$(BINDIR)/$(OUT)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/$(OUT)

clean:
	rm -f $(OUT)

.PHONY: all install uninstall clean
