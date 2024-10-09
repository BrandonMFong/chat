# author: Brando
# date: 7/29/22
#

include external/libs/makefiles/libpaths.mk 
include external/libs/makefiles/platforms.mk 
include external/libs/bflibc/makefiles/uuid.mk 

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
PLATFORM = linux
PACKAGE_MODE = package-linux
endif
ifeq ($(UNAME_S),Darwin)
PLATFORM = macos
PACKAGE_MODE = package-macos
endif

### Global
BIN_PATH = bin
BUILD_PATH = build
CPPLINKS = -lpthread -lncurses $(BF_LIB_C_UUID_FLAGS) -ldl
CPPSTD = -std=c++20
LIBRARIES = external/openssl/libssl.a external/openssl/libcrypto.a 
PACKAGE_NAME = chat

### macOS Variables
IDENTITY =
TEAMID = 
EMAIL =
PW =

FILES = \
interface cipher cipherasymmetric ciphersymmetric \
log user inputbuffer office \
chatroom message chatroomserver packet \
agent agentclient agentserver sealedpacket \
chatroomclient interfaceserver interfaceclient command \
permissions exception \
chat 

### Release settings
R_CPPFLAGS += $(CPPFLAGS) -Isrc/ \
			  -Iexternal/libs/$(BF_LIB_RPATH_RELEASE) \
			  $(CPPSTD)
R_BIN_NAME = chat
R_BUILD_PATH = $(BUILD_PATH)/release
R_MAIN_FILE = src/main.cpp
R_LIBRARIES = external/libs/$(BF_LIB_RPATH_RELEASE_NET) $(LIBRARIES)
R_OBJECTS = $(patsubst %, $(R_BUILD_PATH)/%.o, $(FILES))

### Debug settings
D_ADDR_SANITIZER = -fsanitize=address
D_CPPFLAGS = $(CPPFLAGS) -DDEBUG -g -Isrc/ \
			 -Iexternal/libs/$(BF_LIB_RPATH_DEBUG) \
			 $(D_ADDR_SANITIZER) $(CPPSTD)
D_BIN_NAME = $(R_BIN_NAME)-debug
D_BUILD_PATH = $(BUILD_PATH)/debug
D_MAIN_FILE = $(R_MAIN_FILE)
D_LIBRARIES = external/libs/$(BF_LIB_RPATH_DEBUG_NET) $(LIBRARIES)
D_OBJECTS = $(patsubst %, $(D_BUILD_PATH)/%.o, $(FILES))

### Test settings
T_CPPFLAGS = $(D_CPPFLAGS) -DTESTING
T_BIN_NAME = $(R_BIN_NAME)-test
T_BUILD_PATH = $(BUILD_PATH)/test
T_MAIN_FILE = testbench/tests.cpp
T_MAIN_OBJECT = $(T_BUILD_PATH)/tests.o
T_LIBRARIES = $(D_LIBRARIES)
T_OBJECTS = $(patsubst %, $(T_BUILD_PATH)/%.o, $(FILES))

### Instructions

# Default
build: release

help:
	@echo "Usage:"
	@echo "	make [target] variables"
	@echo ""
	@echo "Target(s):"
	@echo "	clean		cleans build and bin folder"
	@echo "	build 		builds release verions"
	@echo "	release		builds release version"
	@echo "	debug		builds debug version"
	@echo "	package		compresses build"
	@echo ""
	@echo "Variable(s):"
	@echo "	IDENTITY	(macos only) Developer ID common name"
	@echo "	TEAMID 		(macos only) Developer Team ID"
	@echo "	EMAIL 		(macos only) Developer account email"
	@echo "	PW		(macos only) Developer account password"
	@echo ""
	@echo "Example(s):"
	@echo "	Build for release for macOS distribution"
	@echo "		make clean build codesign package notarize staple IDENTITY=\"\" TEAMID=\"\" EMAIL=\"\" PW=\"\""
	@echo "	Build for release for Linux distribution"
	@echo "		make clean build package"

clean:
	rm -rfv $(BUILD_PATH)
	rm -rfv $(BIN_PATH)
	rm -rfv $(PACKAGE_NAME)

## Release build instructions
release: release-setup $(BIN_PATH)/$(R_BIN_NAME)

release-setup:
	@mkdir -p $(R_BUILD_PATH)
	@mkdir -p bin

$(BIN_PATH)/$(R_BIN_NAME): $(R_MAIN_FILE) $(R_OBJECTS) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CPPFLAGS) $(CPPLINKS) 

$(R_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(R_CPPFLAGS)

## Debug build instructions
debug: debug-setup $(BIN_PATH)/$(D_BIN_NAME)

debug-setup:
	@mkdir -p $(D_BUILD_PATH)
	@mkdir -p bin

$(BIN_PATH)/$(D_BIN_NAME): $(D_MAIN_FILE) $(D_OBJECTS) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CPPFLAGS) $(CPPLINKS) 

$(D_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(D_CPPFLAGS)

## Test build instructions
test: test-setup $(BIN_PATH)/$(T_BIN_NAME)
	./$(BIN_PATH)/$(T_BIN_NAME)

test-setup:
	@mkdir -p $(T_BUILD_PATH)
	@mkdir -p bin

$(BIN_PATH)/$(T_BIN_NAME): $(T_MAIN_OBJECT) $(T_OBJECTS) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CPPFLAGS) $(CPPLINKS) 

$(T_MAIN_OBJECT): $(T_MAIN_FILE) testbench/*.hpp
	g++ -c $< -o $@ $(T_CPPFLAGS)

$(T_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(T_CPPFLAGS)

package: $(PACKAGE_MODE)

package-linux: $(PACKAGE_NAME) $(PACKAGE_NAME)/$(R_BIN_NAME)
	zip -r $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).zip $(PACKAGE_NAME)
	tar vczf $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).tar.gz $(PACKAGE_NAME)

package-macos: $(PACKAGE_NAME) $(PACKAGE_NAME)/$(R_BIN_NAME)
	hdiutil create -fs HFS+ -volname Chat -srcfolder $(PACKAGE_NAME) $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

$(PACKAGE_NAME):
	mkdir -p $@

$(PACKAGE_NAME)/$(R_BIN_NAME): $(BIN_PATH)/$(R_BIN_NAME)
	@cp -afv $< $(PACKAGE_NAME)

codesign:
	codesign -s "$(IDENTITY)" --options=runtime $(BIN_PATH)/$(R_BIN_NAME)

notarize:
	xcrun notarytool submit --apple-id "$(EMAIL)" --password "$(PW)" --team-id "$(TEAMID)" --wait $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

staple:
	xcrun stapler staple $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

