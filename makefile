# author: Brando
# date: 7/29/22
#

include external/libs/makefiles/libpaths.mk 

### Global
BUILD_PATH = build
CXXLINKS = -lpthread -lncurses -luuid

FILES = \
server client interface socket \
log user inputbuffer office \
chatroom message chatroomserver packet \
connection agent agentclient agentserver

### Release settings
R_CXXFLAGS += -Isrc/ -Iexternal/libs/$(BF_LIB_RPATH_RELEASE)
R_BIN_NAME = chat
R_BUILD_PATH = $(BUILD_PATH)/release
R_MAIN_FILE = src/main.cpp
R_LIBRARIES = external/libs/$(BF_LIB_RPATH_RELEASE_CPP)
R_OBJECTS = $(patsubst %, $(R_BUILD_PATH)/%.o, $(FILES))

### Debug settings
D_ADDR_SANITIZER = -fsanitize=address
D_CXXFLAGS = -DDEBUG -g -Isrc/ -Iexternal/libs/$(BF_LIB_RPATH_DEBUG) $(D_ADDR_SANITIZER)
D_BIN_NAME = $(R_BIN_NAME)-debug
D_BUILD_PATH = $(BUILD_PATH)/debug
D_MAIN_FILE = $(R_MAIN_FILE)
D_LIBRARIES = external/libs/$(BF_LIB_RPATH_DEBUG_CPP)
D_OBJECTS = $(patsubst %, $(D_BUILD_PATH)/%.o, $(FILES))

### Test settings
T_CXXFLAGS = $(D_CXXFLAGS) -DTESTING
T_BIN_NAME = $(R_BIN_NAME)-test
T_BUILD_PATH = $(BUILD_PATH)/test
T_MAIN_FILE = testbench/tests.cpp
T_LIBRARIES = $(D_LIBRARIES)
T_OBJECTS = $(patsubst %, $(T_BUILD_PATH)/%.o, $(FILES))

### Instructions

# Default
all: release

clean:
	rm -rfv build
	rm -rfv bin

## Release build instructions
release: release-setup bin/$(R_BIN_NAME)

release-setup:
	@mkdir -p $(R_BUILD_PATH)
	@mkdir -p bin

bin/$(R_BIN_NAME): $(R_MAIN_FILE) $(R_OBJECTS) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CXXFLAGS) $(CXXLINKS) 

$(R_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(R_CXXFLAGS)

## Debug build instructions
debug: debug-setup bin/$(D_BIN_NAME)

debug-setup:
	@mkdir -p $(D_BUILD_PATH)
	@mkdir -p bin

bin/$(D_BIN_NAME): $(D_MAIN_FILE) $(D_OBJECTS) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CXXFLAGS) $(CXXLINKS) 

$(D_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(D_CXXFLAGS)

## Test build instructions
test: test-setup bin/$(T_BIN_NAME)
	./bin/$(T_BIN_NAME)

test-setup:
	@mkdir -p $(T_BUILD_PATH)
	@mkdir -p bin

bin/$(T_BIN_NAME): $(T_MAIN_FILE) $(T_OBJECTS) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CXXFLAGS) $(CXXLINKS) 

$(T_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(T_CXXFLAGS)

