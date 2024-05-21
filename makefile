# author: Brando
# date: 7/29/22
#

include external/libs/makefiles/libpaths.mk 
include external/libs/makefiles/platforms.mk 
include external/libs/bflibc/makefiles/uuid.mk 

### Global
BUILD_PATH = build
CPPLINKS = -lpthread -lncurses $(BF_LIB_C_UUID_FLAGS)
CPPSTD = -std=c++20

FILES = \
interface \
log user inputbuffer office \
chatroom message chatroomserver packet \
agent agentclient agentserver \
chatroomclient interfaceserver interfaceclient command \
permissions chat

### Release settings
R_CPPFLAGS += $(CPPFLAGS) -Isrc/ \
			  -Iexternal/libs/$(BF_LIB_RPATH_RELEASE) \
			  $(CPPSTD)
R_BIN_NAME = chat
R_BUILD_PATH = $(BUILD_PATH)/release
R_MAIN_FILE = src/main.cpp
R_LIBRARIES = external/libs/$(BF_LIB_RPATH_RELEASE_CPP) external/libs/$(BF_LIB_RPATH_RELEASE_NET)
R_OBJECTS = $(patsubst %, $(R_BUILD_PATH)/%.o, $(FILES))

### Debug settings
D_ADDR_SANITIZER = -fsanitize=address
D_CPPFLAGS = $(CPPFLAGS) -DDEBUG -g -Isrc/ \
			 -Iexternal/libs/$(BF_LIB_RPATH_DEBUG) \
			 $(D_ADDR_SANITIZER) $(CPPSTD)
D_BIN_NAME = $(R_BIN_NAME)-debug
D_BUILD_PATH = $(BUILD_PATH)/debug
D_MAIN_FILE = $(R_MAIN_FILE)
D_LIBRARIES = external/libs/$(BF_LIB_RPATH_DEBUG_CPP) external/libs/$(BF_LIB_RPATH_DEBUG_NET)
D_OBJECTS = $(patsubst %, $(D_BUILD_PATH)/%.o, $(FILES))

### Test settings
T_CPPFLAGS = $(D_CPPFLAGS) -DTESTING
T_BIN_NAME = $(R_BIN_NAME)-test
T_BUILD_PATH = $(BUILD_PATH)/test
T_MAIN_FILE = testbench/tests.cpp
T_LIBRARIES = $(D_LIBRARIES)
T_OBJECTS = $(patsubst %, $(T_BUILD_PATH)/%.o, $(FILES))

### Instructions

# Default
build: release

all: release debug

clean:
	rm -rfv build
	rm -rfv bin

## Release build instructions
release: release-setup bin/$(R_BIN_NAME)

release-setup:
	@mkdir -p $(R_BUILD_PATH)
	@mkdir -p bin

bin/$(R_BIN_NAME): $(R_MAIN_FILE) $(R_OBJECTS) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CPPFLAGS) $(CPPLINKS) 

$(R_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(R_CPPFLAGS)

## Debug build instructions
debug: debug-setup bin/$(D_BIN_NAME)

debug-setup:
	@mkdir -p $(D_BUILD_PATH)
	@mkdir -p bin

bin/$(D_BIN_NAME): $(D_MAIN_FILE) $(D_OBJECTS) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CPPFLAGS) $(CPPLINKS) 

$(D_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(D_CPPFLAGS)

## Test build instructions
test: test-setup bin/$(T_BIN_NAME)
	./bin/$(T_BIN_NAME)

test-setup:
	@mkdir -p $(T_BUILD_PATH)
	@mkdir -p bin

bin/$(T_BIN_NAME): $(T_MAIN_FILE) $(T_OBJECTS) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CPPFLAGS) $(CPPLINKS) 

$(T_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(T_CPPFLAGS)

