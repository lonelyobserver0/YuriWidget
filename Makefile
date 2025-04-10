# Nome dell'eseguibile
SERVER = yuriwidget
CLIENT = yuriwidget_client

# Directory
SRC_DIR = src
CLIENT_DIR = client
BUILD_DIR = build

# File sorgente
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

CLIENT_SRC = $(CLIENT_DIR)/yuriwidget_client.c
CLIENT_BIN = $(BUILD_DIR)/$(CLIENT)

# Flags di compilazione
CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0` -Wall -g
LDFLAGS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0` -pthread

# Target di default
all: $(BUILD_DIR) $(BUILD_DIR)/$(SERVER) $(CLIENT_BIN)

# Build dell'eseguibile principale
$(BUILD_DIR)/$(SERVER): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Build oggetti
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Build del client CLI
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) -o $@ $<

# Crea la directory build se non esiste
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Pulizia
clean:
	rm -rf $(BUILD_DIR)

# Installazione (copiando nella cartella /usr/local/bin)
install:
	sudo install -m 755 $(BUILD_DIR)/$(SERVER) /usr/local/bin/$(SERVER)
	sudo install -m 755 $(CLIENT_BIN) /usr/local/bin/$(CLIENT)

.PHONY: all clean install
