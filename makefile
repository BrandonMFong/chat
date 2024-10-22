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
LIBRARIES = external/bin/openssl-uni/libssl.a external/bin/openssl-uni/libcrypto.a 
PACKAGE_NAME = chat

# used to make universal binaries
MACOS_TARGET_X86_64 = x86_64-apple-macos10.12
MACOS_TARGET_ARM64 = arm64-apple-macos11

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
ifeq ($(UNAME_S),Darwin)
R_OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(R_BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
R_OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(R_BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
else
R_OBJECTS = $(patsubst %, $(R_BUILD_PATH)/%.o, $(FILES))
endif

### Debug settings
D_ADDR_SANITIZER = -fsanitize=address
D_CPPFLAGS = $(CPPFLAGS) -DDEBUG -g -Isrc/ \
			 -Iexternal/libs/$(BF_LIB_RPATH_DEBUG) \
			 $(D_ADDR_SANITIZER) $(CPPSTD)
D_BIN_NAME = $(R_BIN_NAME)-debug
D_BUILD_PATH = $(BUILD_PATH)/debug
D_MAIN_FILE = $(R_MAIN_FILE)
D_LIBRARIES = external/libs/$(BF_LIB_RPATH_DEBUG_NET) $(LIBRARIES)
ifeq ($(UNAME_S),Darwin)
D_OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(D_BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
D_OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(D_BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
else
D_OBJECTS = $(patsubst %, $(D_BUILD_PATH)/%.o, $(FILES))
endif

### Test settings
T_CPPFLAGS = $(D_CPPFLAGS) -DTESTING
T_BIN_NAME = $(R_BIN_NAME)-test
T_BUILD_PATH = $(BUILD_PATH)/test
T_MAIN_FILE = testbench/tests.cpp
T_LIBRARIES = $(D_LIBRARIES)
ifeq ($(UNAME_S),Darwin)
T_MAIN_OBJECT_MACOS_TARGET_X86_64 = $(T_BUILD_PATH)/tests.$(MACOS_TARGET_X86_64)
T_MAIN_OBJECT_MACOS_TARGET_ARM64 = $(T_BUILD_PATH)/tests.$(MACOS_TARGET_ARM64)
T_OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(T_BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
T_OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(T_BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
else
T_MAIN_OBJECT = $(T_BUILD_PATH)/tests.o
T_OBJECTS = $(patsubst %, $(T_BUILD_PATH)/%.o, $(FILES))
endif

.PRECIOUS: \
	$(R_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(R_BUILD_PATH)/%.$(MACOS_TARGET_ARM64) \
	$(D_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(D_BUILD_PATH)/%.$(MACOS_TARGET_ARM64) \
	$(T_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(T_BUILD_PATH)/%.$(MACOS_TARGET_ARM64)

### Instructions

# Default
build: release

help:
	@echo "Usage:"
	@echo "	make [target] variables"
	@echo ""
	@echo "Target(s):"
	@echo "	clean			cleans build and bin folder"
	@echo "	build 			builds release verions"
	@echo "	release			builds release version"
	@echo "	debug			builds debug version"
	@echo "	package			compresses build"
	@echo "	dependecies		builds all dependencies in the external directory"
	@echo "	clean-dependecies	builds all dependencies in the external directory"
	@echo "	clean-all		cleans local and dependency builds"
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

clean-all: clean clean-dependencies

clean:
	rm -rfv $(BUILD_PATH)
	rm -rfv $(BIN_PATH)
	rm -rfv $(PACKAGE_NAME)

## Release build instructions
release: release-setup dependencies $(BIN_PATH)/$(R_BIN_NAME)

release-setup:
	@mkdir -p $(R_BUILD_PATH)
	@mkdir -p bin

ifeq ($(UNAME_S),Darwin)
$(BIN_PATH)/$(R_BIN_NAME): $(BIN_PATH)/$(R_BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(R_BIN_NAME).$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(BIN_PATH)/$(R_BIN_NAME).$(MACOS_TARGET_X86_64): $(R_MAIN_FILE) $(R_OBJECTS_MACOS_TARGET_X86_64) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_X86_64)

$(BIN_PATH)/$(R_BIN_NAME).$(MACOS_TARGET_ARM64): $(R_MAIN_FILE) $(R_OBJECTS_MACOS_TARGET_ARM64) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_ARM64)

$(R_BUILD_PATH)/%.o: $(R_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(R_BUILD_PATH)/%.$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(R_BUILD_PATH)/%.$(MACOS_TARGET_X86_64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(R_CPPFLAGS) -target $(MACOS_TARGET_X86_64)

$(R_BUILD_PATH)/%.$(MACOS_TARGET_ARM64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(R_CPPFLAGS) -target $(MACOS_TARGET_ARM64)
else
$(BIN_PATH)/$(R_BIN_NAME): $(R_MAIN_FILE) $(R_OBJECTS) $(R_LIBRARIES)
	g++ -o $@ $^ $(R_CPPFLAGS) $(CPPLINKS)

$(R_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(R_CPPFLAGS)
endif

## Debug build instructions
debug: debug-setup dependencies $(BIN_PATH)/$(D_BIN_NAME)

debug-setup:
	@mkdir -p $(D_BUILD_PATH)
	@mkdir -p bin

ifeq ($(UNAME_S),Darwin)
$(BIN_PATH)/$(D_BIN_NAME): $(BIN_PATH)/$(D_BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(D_BIN_NAME).$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(BIN_PATH)/$(D_BIN_NAME).$(MACOS_TARGET_X86_64): $(D_MAIN_FILE) $(D_OBJECTS_MACOS_TARGET_X86_64) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_X86_64)

$(BIN_PATH)/$(D_BIN_NAME).$(MACOS_TARGET_ARM64): $(D_MAIN_FILE) $(D_OBJECTS_MACOS_TARGET_ARM64) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_ARM64)

$(D_BUILD_PATH)/%.o: $(D_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(D_BUILD_PATH)/%.$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(D_BUILD_PATH)/%.$(MACOS_TARGET_X86_64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(D_CPPFLAGS) -target $(MACOS_TARGET_X86_64)

$(D_BUILD_PATH)/%.$(MACOS_TARGET_ARM64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(D_CPPFLAGS) -target $(MACOS_TARGET_ARM64)
else
$(BIN_PATH)/$(D_BIN_NAME): $(D_MAIN_FILE) $(D_OBJECTS) $(D_LIBRARIES)
	g++ -o $@ $^ $(D_CPPFLAGS) $(CPPLINKS)

$(D_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(D_CPPFLAGS)
endif

## Test build instructions
test: test-setup dependencies $(BIN_PATH)/$(T_BIN_NAME)
	./$(BIN_PATH)/$(T_BIN_NAME)

test-setup:
	@mkdir -p $(T_BUILD_PATH)
	@mkdir -p bin

ifeq ($(UNAME_S),Darwin)
$(BIN_PATH)/$(T_BIN_NAME): $(BIN_PATH)/$(T_BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(T_BIN_NAME).$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(BIN_PATH)/$(T_BIN_NAME).$(MACOS_TARGET_X86_64): $(T_MAIN_OBJECT_MACOS_TARGET_X86_64) $(T_OBJECTS_MACOS_TARGET_X86_64) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_X86_64)

$(BIN_PATH)/$(T_BIN_NAME).$(MACOS_TARGET_ARM64): $(T_MAIN_OBJECT_MACOS_TARGET_ARM64) $(T_OBJECTS_MACOS_TARGET_ARM64) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CPPFLAGS) $(CPPLINKS) -target $(MACOS_TARGET_ARM64)

$(T_MAIN_OBJECT_MACOS_TARGET_X86_64): $(T_MAIN_FILE) testbench/*.hpp
	g++ -c $< -o $@ $(T_CPPFLAGS) -target $(MACOS_TARGET_X86_64)

$(T_MAIN_OBJECT_MACOS_TARGET_ARM64): $(T_MAIN_FILE) testbench/*.hpp
	g++ -c $< -o $@ $(T_CPPFLAGS) -target $(MACOS_TARGET_ARM64)

$(T_BUILD_PATH)/%.o: $(T_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(T_BUILD_PATH)/%.$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(T_BUILD_PATH)/%.$(MACOS_TARGET_X86_64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(T_CPPFLAGS) -target $(MACOS_TARGET_X86_64)

$(T_BUILD_PATH)/%.$(MACOS_TARGET_ARM64): src/%.cpp src/%.hpp src/*.h
	g++ -c -o $@ $< $(T_CPPFLAGS) -target $(MACOS_TARGET_ARM64)
else
$(BIN_PATH)/$(T_BIN_NAME): $(T_MAIN_FILE) $(T_OBJECTS) $(T_LIBRARIES)
	g++ -o $@ $^ $(T_CPPFLAGS) $(CPPLINKS)

$(T_BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(T_CPPFLAGS)
endif

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

## Dependencies
dependencies: libs openssl

clean-dependencies:
	cd external/libs && make clean
	rm -rfv external/bin

libs: external/libs/$(BF_LIB_RPATH_RELEASE_NET) external/libs/$(BF_LIB_RPATH_DEBUG_NET)
external/libs/$(BF_LIB_RPATH_RELEASE_NET):
	cd external/libs && make clean all
external/libs/$(BF_LIB_RPATH_DEBUG_NET):
	cd external/libs && make clean all

openssl-setup:
	mkdir -p external/bin/openssl-arm
	mkdir -p external/bin/openssl-intel
	mkdir -p external/bin/openssl-uni

openssl: openssl-setup external/bin/openssl-uni/libssl.a external/bin/openssl-uni/libcrypto.a

external/bin/openssl-uni/libssl.a: external/bin/openssl-arm/libssl.a external/bin/openssl-intel/libssl.a
	lipo -create $^ -output $@
	
external/bin/openssl-uni/libcrypto.a: external/bin/openssl-arm/libcrypto.a external/bin/openssl-intel/libcrypto.a
	lipo -create $^ -output $@

external/bin/openssl-arm/libssl.a:
	cd external/bin/openssl-arm && ../../openssl/Configure darwin64-arm64 CPPFLAGS="-target arm64-apple-macos11" && make
external/bin/openssl-arm/libcrypto.a:
	cd external/bin/openssl-arm && ../../openssl/Configure darwin64-arm64 CPPFLAGS="-target arm64-apple-macos11" && make

external/bin/openssl-intel/libssl.a:
	cd external/bin/openssl-intel && ../../openssl/Configure darwin64-x86_64 CPPFLAGS="-target x86_64-apple-macos10.12" && make
external/bin/openssl-intel/libcrypto.a:
	cd external/bin/openssl-intel && ../../openssl/Configure darwin64-x86_64 CPPFLAGS="-target x86_64-apple-macos10.12" && make

