# Nome dell'eseguibile
TARGET = yuriwidget

# Directory dei sorgenti e di output
SRC_DIR = src
BUILD_DIR = build

# File sorgente
SRC = $(SRC_DIR)/yuriwidget.c

# Flags per GTK+ 3 e WebKit2GTK
CFLAGS = `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`
LIBS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0` -pthread

# Comando di compilazione
CC = gcc

# Regola principale
all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LIBS)

# Pulizia dei file compilati
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
