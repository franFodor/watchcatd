# Compiler flags
CFLAGS = -Wall -Werror -std=gnu99 -pedantic

# Source file
SRC = src/watchcatd.c

# Output executable
OUT = watchcatd

# Default target
all: $(OUT)

$(OUT): $(SRC)
	gcc $(CFLAGS) $(SRC) -o $(OUT) `pkg-config --cflags --libs libnotify` 
clean:
	rm -f $(OUT)

.PHONY: all clean
