CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`
LDFLAGS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0` -pthread
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
SRC_FILES = $(SRC_DIR)/yuriwidget.c
OUT_FILE = $(BUILD_DIR)/yuriwidget
LOG_FILE = ~/.cache/yuriwidget.log
SOCKET_PATH = /tmp/yuriwidget.sock

all: $(OUT_FILE)

$(OUT_FILE): $(SRC_FILES)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC_FILES) -o $(OUT_FILE) $(LDFLAGS)
	@echo "[yuriwidget] Build complete."

install:
	@mkdir -p $(BIN_DIR)
	cp $(OUT_FILE) $(BIN_DIR)/yuriwidget
	@echo "[yuriwidget] Installed to $(BIN_DIR)/yuriwidget."

run: $(OUT_FILE)
	$(OUT_FILE)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(BIN_DIR)/yuriwidget
	rm -f $(SOCKET_PATH)
	@echo "[yuriwidget] Cleaned up build and binaries."

log:
	@echo "[yuriwidget] Checking logs in $(LOG_FILE)"
	cat $(LOG_FILE)

.PHONY: all install run clean log
