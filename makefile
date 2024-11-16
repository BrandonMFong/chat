# author: Brando
# date: 7/29/22
#

include external/libs/bflibc/makefiles/uuid.mk 

help:
	@echo "Usage:"
	@echo "	make <target> <variables>"
	@echo ""
	@echo "Target(s):"
	@echo "	clean			cleans build and bin folder"
	@echo "	build 			builds release verions"
	@echo "	package			compresses build"
	@echo "	dependencies		builds all dependencies in the external directory"
	@echo "	clean-dependencies	builds all dependencies in the external directory"
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

COMPILER = g++
CPPSTD = -std=c++20
CONFIG = release
BUILD_TYPE = executable
SOURCE_EXT = cpp
HEADER_EXT = hpp
FILES = \
interface cipher cipherasymmetric ciphersymmetric \
log user inputbuffer office \
chatroom message chatroomserver packet \
agent agentclient agentserver sealedpacket \
chatroomclient interfaceserver interfaceclient command \
permissions chat 

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
LIBRARIES = external/bin/openssl-uni/libssl.a external/bin/openssl-uni/libcrypto.a
UNAME_P := $(shell uname -p)
ifeq ($(UNAME_P),x86_64)
OPENSSL_INCLUDE_PATH = -Iexternal/bin/openssl-intel/include
else ifneq ($(filter arm%,$(UNAME_P)),)
OPENSSL_INCLUDE_PATH = -Iexternal/bin/openssl-arm/include
endif
else
LIBRARIES = external/bin/openssl/libssl.a external/bin/openssl/libcrypto.a
OPENSSL_INCLUDE_PATH = -Iexternal/bin/openssl/include
endif

ifneq ($(CONFIG),test) # test
LIBRARIES += \
	external/bin/libs/$(CONFIG)/bflibc/libbfc.a \
	external/bin/libs/$(CONFIG)/bflibcpp/libbfcpp.a \
	external/bin/libs/$(CONFIG)/bfnet/libbfnet.a
endif

LINKS = -lpthread -lncurses $(BF_LIB_C_UUID_FLAGS) -ldl

### Release settings
ifeq ($(CONFIG),release) # release
MAIN_FILE = src/main.cpp
BIN_NAME = chat
FLAGS = $(CPPFLAGS) -Isrc/ $(CPPSTD) -Iexternal/bin/libs/release $(OPENSSL_INCLUDE_PATH)

### Debug settings
else ifeq ($(CONFIG),debug) # debug
MAIN_FILE = src/main.cpp
BIN_NAME = chat
#ADDR_SANITIZER = -fsanitize=address
FLAGS = $(CPPFLAGS) -DDEBUG -g -Isrc/ $(ADDR_SANITIZER) $(CPPSTD) -Iexternal/bin/libs/debug $(OPENSSL_INCLUDE_PATH)

### Test settings
else ifeq ($(CONFIG),test) # test
MAIN_FILE = testbench/tests.cpp
BIN_NAME = chat-test
#ADDR_SANITIZER = -fsanitize=address
FLAGS = $(CPPFLAGS) -DDEBUG -DTESTING -g -Isrc/ $(ADDR_SANITIZER) $(CPPSTD) -Iexternal/bin/libs/debug $(OPENSSL_INCLUDE_PATH)
LIBRARIES += \
	external/bin/libs/debug/bflibc/libbfc-debug.a \
	external/bin/libs/debug/bflibcpp/libbfcpp-debug.a \
	external/bin/libs/debug/bfnet/libbfnet-debug.a \
	external/bin/libs/debug/bftest/libbftest-debug.a
endif # ($(CONFIG),...)

LIBS_MAKEFILES_PATH:=$(CURDIR)/external/libs/makefiles
include $(LIBS_MAKEFILES_PATH)/build.mk 

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
	xcrun notarytool \
	submit \
	--apple-id "$(EMAIL)" \
	--password "$(PW)" \
	--team-id "$(TEAMID)" \
	--wait \
	$(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

staple:
	xcrun stapler staple $(BIN_PATH)/$(PACKAGE_NAME)-$(PLATFORM).dmg

### Dependencies

dependencies:
	cd external && make build

clean-dependencies:
	cd external && make clean

clean-all: clean clean-dependencies

