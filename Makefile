ifeq ($(shell uname), Darwin)
	LIB_EXT = dylib
else
	LIB_EXT = so
endif

EXTENSION_NAME = window_median_extension

SQLITE_INCLUDE_DIR = /path/to/sqlite/include
SRC_DIR = .

CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -Wno-unused-parameter

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRC_FILES))

.PHONY: all
all: $(EXTENSION_NAME).$(LIB_EXT)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I$(SQLITE_INCLUDE_DIR)

$(EXTENSION_NAME).dylib: $(OBJ_FILES)
	$(CC) -dynamiclib -o $@ $^ -lsqlite3

.PHONY: copy-lib
copy-lib:
	$(shell mkdir ./build)
	cp ./$(EXTENSION_NAME).$(LIB_EXT) ./build/$(EXTENSION_NAME).$(LIB_EXT)

.PHONY: clean
clean:
	rm -f $(SRC_DIR)/*.o $(EXTENSION_NAME).dylib