FRAMEWORK =  -framework CoreServices
BUILD_FLAGS = -std=c99 -Wall -g -O0 -mmacosx-version-min=10.13 #Only tested on 11.0
BUILD_PATH = ./bin
BIN = $(BUILD_PATH)/macdict

.PHONY: all clean install

all: clean $(BIN)

install: clean $(BIN)

clean:
	rm -rf $(BUILD_PATH)

$(BIN): ./src/*.c
	mkdir -p $(BUILD_PATH)
	clang -o $@ $(BUILD_FLAGS) $(FRAMEWORK) $^
