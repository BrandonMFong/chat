##
# author: brando
# date: 11/15/24
#

### Dependencies

build: libs openssl

clean: clean-libs clean-openssl
	rm -rfv bin

clean-libs:
	cd libs && make clean

clean-openssl:
	cd openssl && make clean

## Libs

LIBS = bflibc bflibcpp bfnet bftest
libs: $(LIBS)
$(LIBS):
	cd libs && make build $@

## OpenSSL

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
SETUP_OPENSSL_DIRS = bin/openssl-arm bin/openssl-intel bin/openssl-uni
setup-openssl: $(SETUP_OPENSSL_DIRS)
$(SETUP_OPENSSL_DIRS):
	mkdir -p $@

openssl: setup-openssl bin/openssl-uni/libssl.a bin/openssl-uni/libcrypto.a

bin/openssl-uni/libssl.a: bin/openssl-arm/libssl.a bin/openssl-intel/libssl.a
	lipo -create $^ -output $@
	
bin/openssl-uni/libcrypto.a: bin/openssl-arm/libcrypto.a bin/openssl-intel/libcrypto.a
	lipo -create $^ -output $@

bin/openssl-arm/libssl.a:
	cd bin/openssl-arm && ../../openssl/Configure darwin64-arm64 CPPFLAGS="-target arm64-apple-macos11" && make
bin/openssl-arm/libcrypto.a:
	cd bin/openssl-arm && ../../openssl/Configure darwin64-arm64 CPPFLAGS="-target arm64-apple-macos11" && make

bin/openssl-intel/libssl.a:
	cd bin/openssl-intel && ../../openssl/Configure darwin64-x86_64 CPPFLAGS="-target x86_64-apple-macos10.12" && make
bin/openssl-intel/libcrypto.a:
	cd bin/openssl-intel && ../../openssl/Configure darwin64-x86_64 CPPFLAGS="-target x86_64-apple-macos10.12" && make
else # ($(UNAME_S),???)
openssl: openssl/libssl.a openssl/libcrypto.a
openssl/libssl.a:
	cd openssl && ./Configure && make
openssl/libcrypto.a:
	cd openssl && ./Configure && make
endif # ($(UNAME_S),???)


