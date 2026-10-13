CC = gcc
PKGS = sdl2 SDL2_image ncursesw
CFLAGS = -std=c99 -Wall -Wextra -g -Isrc $(shell pkg-config --cflags $(PKGS))
LDLIBS = $(shell pkg-config --libs $(PKGS))

SRC = $(wildcard src/*.c src/model/*.c src/controller/*.c src/view/*.c)
OBJ = $(SRC:src/%.c=build/%.o)
TARGET = tron

# Version navigateur : le modèle seul, compilé en WebAssembly par clang (sans bibliothèque C, voir src/web)
WEB_DIR = build/web
WEB_SRC = src/model/model.c src/web/tron_web.c
WEB_CFLAGS = --target=wasm32 -O2 -Wall -Wextra -nostdlib -fno-builtin -isystem src/web/libc -Isrc -Wl,--no-entry -Wl,--strip-all

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDLIBS)

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Le modèle ne dépend d'aucune bibliothèque : ses tests se compilent sans SDL ni ncurses
test:
	@mkdir -p build
	$(CC) -std=c99 -Wall -Wextra -g -Isrc src/model/model.c tests/test_model.c -o build/test_model
	./build/test_model

# Le .wasm est embarqué en base64 dans un script : la page se charge alors sans serveur web
web:
	@mkdir -p build $(WEB_DIR)
	clang $(WEB_CFLAGS) $(WEB_SRC) -o build/tron.wasm
	@echo '// Généré par `make web` dans le dépôt tron : le modèle C compilé en WebAssembly. Ne pas modifier.' > $(WEB_DIR)/tron.js
	@echo "window.TRON_WASM = \"$$(base64 -w0 build/tron.wasm)\";" >> $(WEB_DIR)/tron.js
	@echo "$(WEB_DIR)/tron.js"

clean:
	rm -rf build $(TARGET)

-include $(OBJ:.o=.d)

.PHONY: all test web clean
