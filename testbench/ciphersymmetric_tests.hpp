/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CIPHER_SYMMETRIC_TESTS_HPP
#define CIPHER_SYMMETRIC_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "cipher.hpp"
#include "log.hpp"
#include "ciphersymmetric.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_SymCheckReady() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 2;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->isReady()) {
			result = 5;
		} else if (c->genkey()) {
			result = 2;
		} else if (!c->isReady()) {
			result = 4;
		}

		const char * str = "Hello world!";
		Data plain(strlen(str)+1, (unsigned char *) str);
		Data enc;
		if (!result) {
			result = c->encrypt(plain, enc);
		}
		
		Data dec;
		if (!result) {
			result = c->decrypt(enc, dec);
		}

		if (!result) {
			String res = dec;
			if (strcmp(res.cString(), str)) {
				printf("%s != %s\n", res, str);
				result = 3;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}


int test_SimpleString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->genkey()) {
			result = 2;
		}

		const char * str = "Hello world!";
		Data plain(strlen(str)+1, (unsigned char *) str);
		Data enc;
		if (!result) {
			result = c->encrypt(plain, enc);
		}

		Data dec;
		if (!result) {
			result = c->decrypt(enc, dec);
		}

		if (!result) {
			String res = dec;
			if (strcmp(res.cString(), str)) {
				printf("%s != %s\n", res.cString(), str);
				result = 2;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_EmptyString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->genkey()) {
			result = 2;
		}

		String str = "";
		Data plain(str);
		Data enc;
		if (!result) {
			result = c->encrypt(plain, enc);
		}

		Data dec;
		if (!result) {
			result = c->decrypt(enc, dec);
		}

		if (!result) {
			String res = dec;
			if (strcmp(res.cString(), str)) {
				printf("%s != %s\n", res.cString(), str);
				result = 2;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}
int test_LongString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->genkey()) {
			result = 2;
		}

		String str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
		Data plain(str);
		Data enc;
		if (!result) {
			result = c->encrypt(plain, enc);
		}

		Data dec;
		if (!result) {
			result = c->decrypt(enc, dec);
		}

		if (!result) {
			String res = dec;
			if (strcmp(res.cString(), str)) {
				printf("%s != %s\n", res.cString(), str);
				result = 2;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_RandomBytes() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->genkey()) {
			result = 2;
		}

		size_t size = DATA_BUFFER_SIZE;
		unsigned char * buf = (unsigned char *) malloc(size);
		Data plain(size, buf);
		Data enc;
		if (!result) {
			result = c->encrypt(plain, enc);
		}

		Data dec;
		if (!result) {
			result = c->decrypt(enc, dec);
		}

		if (!result) {
			if (memcmp(dec.buffer(), buf, size)) {
				result = 4;
			}
		}

		BFFree(buf);
		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_HandingOffKey() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		CipherSymmetric * u1 = (CipherSymmetric *) Cipher::create(kCipherTypeSymmetric);
		CipherSymmetric * u2 = (CipherSymmetric *) Cipher::create(kCipherTypeSymmetric);
		Data key;
		int err = 0;
		if (u1 == 0) {
		} else if (u2 == 0) {
			result = 1;
		} else if ((err = u1->genkey()) != 0) {
			result = err;
		} else if ((err = u1->getkey(key)) != 0) {
			result = err;
		} else if ((err = u2->setkey(key)) != 0) {
			result = err;
		}

		String str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
		Data plain(str);
		Data enc;
		if (!result) {
			result = u1->encrypt(plain, enc);
		}

		Data dec;
		if (!result) {
			result = u2->decrypt(enc, dec);
		}

		if (!result) {
			String res = dec;
			if (str != res) {
				printf("%s != %s\n", res, str);
				result = 2;
			}
		}

		BFDelete(u1);
		BFDelete(u2);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void ciphersymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;
	
	LOG_OPEN;

	LAUNCH_TEST(test_SimpleString, p, f);
	LAUNCH_TEST(test_LongString, p, f);
	LAUNCH_TEST(test_HandingOffKey, p, f);
	LAUNCH_TEST(test_RandomBytes, p, f);
	LAUNCH_TEST(test_EmptyString, p, f);
	LAUNCH_TEST(test_SymCheckReady, p, f);
	
	LOG_FLUSH;
	LOG_CLOSE;

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_SYMMETRIC_TESTS_HPP

