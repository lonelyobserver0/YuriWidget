# Nome del programma
TARGET = build/myVandal

# Compilatore
CC = gcc

# Opzioni di compilazione
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 webkit2gtk-4.0`

# Opzioni di collegamento
LDFLAGS = `pkg-config --libs gtk+-3.0 webkit2gtk-4.0`

# Directory dei sorgenti e dei file oggetto
SRC_DIR = src
BUILD_DIR = build

# File sorgente
SRCS = $(wildcard $(SRC_DIR)/*.c)

# File oggetto
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Regola principale
all: $(TARGET)

# Regola per creare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Regola per compilare i file sorgente in file oggetto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regola per pulire i file generati
clean:
	rm -f $(OBJS) $(TARGET)

# Regola per eseguire il programma
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
