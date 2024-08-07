# Compiler flags
CFLAGS = -Wall -Werror

# Source file
SRC = src/watchcatd.c

# Output executable
OUT = watchcatd

# Default target
all: $(OUT)

$(OUT): $(SRC)
	gcc $(CFLAGS) -o $(OUT) $(SRC)

clean:
	rm -f $(OUT)

.PHONY: all clean
