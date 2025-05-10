CC = cc
CFLAGS = -Ideps/tomlc99 `pkg-config --cflags gtk4 gtk-layer-shell`
LDLIBS = `pkg-config --libs gtk4 gtk-layer-shell`
SRC = yuriwidget.c
BIN = yuriwidget

.PHONY: all deps clean

all: $(BIN)

$(BIN): $(SRC)
	$(CC) -o $@ $^ $(CFLAGS) $(LDLIBS)

deps:
	@echo "Controllo delle dipendenze..."
	@pkg-config --exists gtk4 || { echo "gtk4 non trovato, installazione..."; sudo pacman -Sy --noconfirm gtk4; }
	@pkg-config --exists gtk-layer-shell || { echo "gtk-layer-shell non trovato, installazione..."; sudo pacman -Sy --noconfirm gtk-layer-shell; }

clean:
	rm -f $(BIN)
