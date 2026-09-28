CC ?= gcc
CFLAGS ?= -Wall -Wextra -O2 -I/usr/include/quickjs -Isrc
LDFLAGS ?= -L/usr/lib/aarch64-linux-gnu/quickjs -lquickjs -lm -lpthread -luv

SRC = src/main.c src/bindings/console.c src/bindings/process.c
OBJ = $(SRC:.c=.o)
TARGET = bin/micro-node

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p bin
	$(CC) $(OBJ) -o $@ $(LDFLAGS)
	@echo "✓ Berhasil membangun: $(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin $(OBJ)
	@echo "✓ Direktori bersih."

test: $(TARGET)
	@echo "--- Menjalankan Test 1: Hello World ---"
	./$(TARGET) examples/01_hello.js
	@echo "\n--- Menjalankan Test 2: Process Object ---"
	./$(TARGET) examples/02_process.js foo bar 123

.PHONY: all clean test
