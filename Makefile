APP_NAME = yuriwidget
SRC = yuriwidget.c deps/tomlc99/toml.c
DEPS_DIR = deps
TOMLC99_DIR = $(DEPS_DIR)/tomlc99

CFLAGS = -I$(TOMLC99_DIR)
LDLIBS = `pkg-config --cflags --libs gtk4`

.PHONY: all run clean deps check_deps install_deps

all: check_deps deps $(APP_NAME)

$(APP_NAME): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS) $(LDLIBS)

deps:
	@if [ ! -d "$(TOMLC99_DIR)" ]; then \
		echo "Cloning tomlc99..."; \
		mkdir -p $(DEPS_DIR); \
		git clone https://github.com/cktan/tomlc99.git $(TOMLC99_DIR); \
	else \
		echo "tomlc99 already present."; \
	fi

check_deps:
	@command -v pkg-config >/dev/null 2>&1 || { echo "pkg-config not found. Installing..."; sudo pacman -S --noconfirm pkgconf; }
	@pkg-config --exists gtk4 || { echo "gtk4 not found. Installing..."; sudo pacman -S --noconfirm gtk4; }

run: all
	./$(APP_NAME) --config-file config.toml

clean:
	rm -f $(APP_NAME)
