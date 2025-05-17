CC = gcc
CFLAGS = -Wall
TARGET = yuriwidget
SRC = yuriwidget.c

# Ottieni i flag di compilazione e di collegamento
CFLAGS += $(shell pkg-config --cflags gtk4 gtk-layer-shell)
LDFLAGS = $(shell pkg-config --libs gtk4 gtk-layer-shell)

# Controlla se pkg-config ha restituito errori
ifeq ($(CFLAGS),)
    $(error "GTK4 and/or gtk-layer-shell not found. Please install them.")
endif

all: $(TARGET)

$(TARGET): $(SRC)
	@echo "Compiling with flags: $(CFLAGS)"
	@echo "Linking with flags: $(LDFLAGS)"
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)
