
# compiler flags
CC := clang
CFLAGS := --target=wasm32 --no-standard-libraries -Wall
LD := wasm-ld
LFLAGS := --no-entry --export-all #--allow-undefined

# source
TARGET := renderer.wasm

SRC_DIR := src
BIN_DIR := bin
OBJ_DIR := $(BIN_DIR)/obj

SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
HDR_FILES := $(wildcard $(SRC_DIR)/*.h)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HDR_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES) Makefile
	@mkdir -p $(BIN_DIR)
	$(LD) $(LFLAGS) $(OBJ_FILES) -o $(BIN_DIR)/$@

all: $(TARGET)

clean:
	@rm -rf $(BIN_DIR)
