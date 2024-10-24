# author: Brando
# date: 7/29/22
#

include external/libs/makefiles/libpaths.mk 
include external/libs/makefiles/platforms.mk 
include external/libs/bflibc/makefiles/uuid.mk 

.SECONDEXPANSION:

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
PLATFORM = linux
PACKAGE_MODE = package-linux
endif
ifeq ($(UNAME_S),Darwin)
PLATFORM = macos
PACKAGE_MODE = package-macos
endif

CONFIG = release
BIN_PATH = bin
CPPLINKS = -lpthread -lncurses $(BF_LIB_C_UUID_FLAGS) -ldl
CPPSTD = -std=c++20
PACKAGE_NAME = chat
MAIN_FILE = src/main.cpp

ifeq ($(UNAME_S),Darwin)
LIBRARIES = external/bin/openssl-uni/libssl.a external/bin/openssl-uni/libcrypto.a 
else
LIBRARIES = external/openssl/libssl.a external/openssl/libcrypto.a 
endif

# used to make universal binaries
MACOS_TARGET_X86_64 = x86_64-apple-macos10--12
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
ifeq ($(CONFIG), release) # ($(CONFIG), ???)
CPPFLAGS += -Isrc/ \
	-Iexternal/libs/$(BF_LIB_RPATH_RELEASE) \
	$(CPPSTD)
BIN_NAME = chat
BUILD_PATH = build/release
LIBRARIES += external/libs/$(BF_LIB_RPATH_RELEASE_NET)
ifeq ($(UNAME_S),Darwin)
OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
OBJECTS_MACOS_TARGETS = $(OBJECTS_MACOS_TARGET_X86_64) $(OBJECTS_MACOS_TARGET_ARM64)
BIN_MACOS_TARGETS = $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_ARM64)
else # ($(UNAME_S),Darwin)
OBJECTS = $(patsubst %, $(BUILD_PATH)/%.o, $(FILES))
endif # ($(UNAME_S),Darwin)
### Debug settings
else ifeq ($(CONFIG), debug) # ($(CONFIG), ???)
ADDR_SANITIZER = -fsanitize=address
CPPFLAGS += -DDEBUG -g -Isrc/ \
	-Iexternal/libs/$(BF_LIB_RPATH_DEBUG) \
	$(ADDR_SANITIZER) $(CPPSTD)
BIN_NAME = chat-debug
BUILD_PATH = build/debug
LIBRARIES += external/libs/$(BF_LIB_RPATH_DEBUG_NET)
ifeq ($(UNAME_S),Darwin)
OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
OBJECTS_MACOS_TARGETS = $(OBJECTS_MACOS_TARGET_X86_64) $(OBJECTS_MACOS_TARGET_ARM64)
BIN_MACOS_TARGETS = $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_ARM64)
else # ($(UNAME_S),Darwin)
D_OBJECTS = $(patsubst %, $(BUILD_PATH)/%.o, $(FILES))
endif # ($(UNAME_S),Darwin)
### Test settings
else ifeq ($(CONFIG), test) # ($(CONFIG), ???)
ADDR_SANITIZER = -fsanitize=address
CPPFLAGS += -DDEBUG -DTESTING -g -Isrc/ \
	-Iexternal/libs/$(BF_LIB_RPATH_DEBUG) \
	$(ADDR_SANITIZER) $(CPPSTD)
BIN_NAME = chat-test
BUILD_PATH = build/test
MAIN_FILE = testbench/tests.cpp
LIBRARIES += external/libs/$(BF_LIB_RPATH_DEBUG_NET)
BIN_PREREQS := $(wildcard testbench/*.hpp)
ifeq ($(UNAME_S),Darwin)
MAIN_OBJECT_MACOS_TARGET_X86_64 = $(BUILD_PATH)/tests.$(MACOS_TARGET_X86_64)
MAIN_OBJECT_MACOS_TARGET_ARM64 = $(BUILD_PATH)/tests.$(MACOS_TARGET_ARM64)
MAIN_OBJECT_MACOS_TARGETS = $(MAIN_OBJECT_MACOS_TARGET_X86_64) $(MAIN_OBJECT_MACOS_TARGET_ARM64)
OBJECTS_MACOS_TARGET_X86_64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_X86_64), $(FILES))
OBJECTS_MACOS_TARGET_ARM64 = $(patsubst %, $(BUILD_PATH)/%.$(MACOS_TARGET_ARM64), $(FILES))
OBJECTS_MACOS_TARGETS = $(OBJECTS_MACOS_TARGET_X86_64) $(OBJECTS_MACOS_TARGET_ARM64)
BIN_MACOS_TARGETS = $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_X86_64) $(BIN_PATH)/$(BIN_NAME).$(MACOS_TARGET_ARM64)
else
MAIN_OBJECT = $(BUILD_PATH)/tests.o
OBJECTS = $(patsubst %, $(BUILD_PATH)/%.o, $(FILES))
endif
endif # ($(CONFIG), ???)

.PRECIOUS: \
	$(R_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(R_BUILD_PATH)/%.$(MACOS_TARGET_ARM64) \
	$(D_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(D_BUILD_PATH)/%.$(MACOS_TARGET_ARM64) \
	$(T_BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(T_BUILD_PATH)/%.$(MACOS_TARGET_ARM64)

build: setup dependencies $(BIN_PATH)/$(BIN_NAME)

run:
	./$(BIN_PATH)/$(BIN_NAME)

help:
	@echo "Usage:"
	@echo "	make [target] variables"
	@echo ""
	@echo "Target(s):"
	@echo "	clean			cleans build and bin folder"
	@echo "	build 			builds release verions"
	@echo "	package			compresses build"
	@echo "	dependecies		builds all dependencies in the external directory"
	@echo "	clean-dependecies	builds all dependencies in the external directory"
	@echo "	clean-all		cleans local and dependency builds"
	@echo ""
	@echo "Variable(s):"
	@echo "	CONFIG		use this to change the build config. Accepts \"release\" (default), \"debug\", or \"test\""
	@echo "	IDENTITY	(macos only) \"Developer ID Application\" common name"
	@echo "	TEAMID 		(macos only) Organizational Unit"
	@echo "	EMAIL 		(macos only) Developer account email"
	@echo "	PW		(macos only) Developer account password"
	@echo ""
	@echo "Example(s):"
	@echo "	Build for release for macOS distribution"
	@echo "		make clean build codesign package notarize staple IDENTITY=\"\" TEAMID=\"\" EMAIL=\"\" PW=\"\""
	@echo "	Build for release for Linux distribution"
	@echo "		make clean build package"

SETUP_DIRS = $(BIN_PATH) $(BUILD_PATH)
setup: $(SETUP_DIRS)
$(SETUP_DIRS):
	mkdir -p $@

clean-all: clean clean-dependencies

clean:
	rm -rfv $(BUILD_PATH)
	rm -rfv $(BIN_PATH)
	rm -rfv $(PACKAGE_NAME)

### Main build

ifeq ($(UNAME_S),Darwin)
$(BIN_PATH)/$(BIN_NAME): $(BIN_MACOS_TARGETS)
	lipo -create -output $@ $^

$(BIN_MACOS_TARGETS): $(MAIN_FILE) $(OBJECTS_MACOS_TARGETS) $(BIN_PREREQS)
	g++ -o $@ $< $(wildcard $(BUILD_PATH)/*$(suffix $@)) $(CPPFLAGS) $(CPPLINKS) $(LIBRARIES) -target $(subst --,.,$(subst .,,$(suffix $@)))

$(BUILD_PATH)/%.o: $(BUILD_PATH)/%.$(MACOS_TARGET_X86_64) $(BUILD_PATH)/%.$(MACOS_TARGET_ARM64)
	lipo -create -output $@ $^

$(OBJECTS_MACOS_TARGETS): $$(subst $(BUILD_PATH), src, $$(subst $$(suffix $$@),, $$@)).cpp  $$(subst $(BUILD_PATH), src, $$(subst $$(suffix $$@),, $$@)).hpp src/*.h
	g++ -c -o $@ $< $(CPPFLAGS) -target $(subst --,.,$(subst .,,$(suffix $@)))

else # ($(UNAME_S),Darwin)
$(BIN_PATH)/$(BIN_NAME): $(MAIN_FILE) $(OBJECTS)
	g++ -o $@ $^ $(LIBRARIES) $(CPPFLAGS) $(CPPLINKS)

$(BUILD_PATH)/%.o: src/%.cpp src/%.hpp src/*.h
	g++ -c $< -o $@ $(CPPFLAGS)
endif

### Packaging

package: $(PACKAGE_MODE)

package-linux: $(PACKAGE_NAME) $(PACKAGE_NAME)/$(BIN_NAME)
	zip -r $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).zip $(PACKAGE_NAME)
	tar vczf $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).tar.gz $(PACKAGE_NAME)

package-macos: $(PACKAGE_NAME) $(PACKAGE_NAME)/$(BIN_NAME)
	hdiutil create -fs HFS+ -volname Chat -srcfolder $(PACKAGE_NAME) $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

$(PACKAGE_NAME):
	mkdir -p $@

$(PACKAGE_NAME)/$(BIN_NAME): $(BIN_PATH)/$(BIN_NAME)
	@cp -afv $< $(PACKAGE_NAME)

codesign:
	codesign -s "$(IDENTITY)" --options=runtime --timestamp $(BIN_PATH)/$(BIN_NAME)

notarize:
	xcrun notarytool submit --apple-id "$(EMAIL)" --password "$(PW)" --team-id "$(TEAMID)" --wait $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

staple:
	xcrun stapler staple $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

### Dependencies

dependencies: libs openssl

clean-dependencies:
	cd external/libs && make clean
	cd external/openssl && make clean
	rm -rfv external/bin

libs: external/libs/$(BF_LIB_RPATH_RELEASE_NET) external/libs/$(BF_LIB_RPATH_DEBUG_NET)
external/libs/$(BF_LIB_RPATH_RELEASE_NET):
	cd external/libs && make clean all
external/libs/$(BF_LIB_RPATH_DEBUG_NET):
	cd external/libs && make clean all

ifeq ($(UNAME_S),Darwin)
SETUP_OPENSSL_DIRS = external/bin/openssl-arm external/bin/openssl-intel external/bin/openssl-uni
setup-openssl: $(SETUP_OPENSSL_DIRS)
$(SETUP_OPENSSL_DIRS):
	mkdir -p $@

openssl: setup-openssl external/bin/openssl-uni/libssl.a external/bin/openssl-uni/libcrypto.a

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
else # ($(UNAME_S),???)
openssl: external/openssl/libssl.a external/openssl/libcrypto.a
external/openssl/libssl.a:
	cd external/openssl && ./Configure && make
external/openssl/libcrypto.a:
	cd external/openssl && ./Configure && make
endif # ($(UNAME_S),???)


