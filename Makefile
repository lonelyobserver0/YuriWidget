CC = cc
CFLAGS = -Ideps/tomlc99 `pkg-config --cflags gtk4 gtk-layer-shell`
LDLIBS = `pkg-config --libs gtk4 gtk-layer-shell`
SRC = yuriwidget.c
BIN = yuriwidget

.PHONY: all check-deps install-deps clean

all: check-deps $(BIN)

check-deps:
	@pkg-config --exists gtk4 || $(MAKE) install-deps
	@pkg-config --exists gtk-layer-shell || $(MAKE) install-deps

install-deps:
	@echo "Installazione delle dipendenze mancanti con pacman..."
	@sudo pacman -Sy --noconfirm gtk4 gtk-layer-shell

$(BIN): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

clean:
	rm -f $(BIN)
