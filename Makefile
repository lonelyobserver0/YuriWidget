APP_NAME = yuriwidget
SRC = yuriwidget.c
TOML_DIR = deps/tomlc99
TOML_REPO = https://raw.githubusercontent.com/cktan/tomlc99/master
TOML_FILES = toml.h toml.c

CFLAGS = `pkg-config --cflags gtk4` -I$(TOML_DIR)
LDFLAGS = `pkg-config --libs gtk4`
OBJ = $(SRC:.c=.o)

all: check-deps toml $(APP_NAME)

$(APP_NAME): $(OBJ) $(TOML_DIR)/toml.c
	$(CC) -o $@ $(OBJ) $(TOML_DIR)/toml.c $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

.PHONY: toml
toml:
	@mkdir -p $(TOML_DIR)
	@for file in $(TOML_FILES); do \
		if [ ! -f "$(TOML_DIR)/$$file" ]; then \
			echo "Scarico $$file..."; \
			curl -sSfL "$(TOML_REPO)/$$file" -o "$(TOML_DIR)/$$file"; \
		fi \
	done

.PHONY: check-deps
check-deps:
	@command -v pkg-config >/dev/null || { echo "Errore: pkg-config mancante."; exit 1; }
	@pkg-config --exists gtk4 || { \
		echo "Errore: GTK 4 non trovato."; \
		echo "Su Arch Linux puoi installarlo con: sudo pacman -S gtk4"; \
		exit 1; \
	}
	@command -v curl >/dev/null || { echo "Errore: curl mancante (necessario per scaricare tomlc99)."; exit 1; }

.PHONY: clean
clean:
	rm -f $(APP_NAME) $(OBJ)
