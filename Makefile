CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0`
SRC_DIR = src
BUILD_DIR = build
SRC = $(SRC_DIR)/yuriwidget.c
OUT = $(BUILD_DIR)/yuriwidget

all: $(BUILD_DIR) $(OUT)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OUT): $(SRC)
	$(CC) -o $(OUT) $(SRC) $(CFLAGS) $(LIBS) -pthread

clean:
	rm -rf $(BUILD_DIR)

install: $(OUT)
	install -Dm755 $(OUT) /usr/local/bin/yuriwidget
