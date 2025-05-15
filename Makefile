CC = gcc
CFLAGS = -Wall -Wextra -O2 `pkg-config --cflags gtk4 gtk-layer-shell`
LDFLAGS = `pkg-config --libs gtk4 gtk-layer-shell`
SRC = yuriwidget.c deps/tomlc99/toml.c
OUT = yuriwidget

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

.PHONY: all clean
