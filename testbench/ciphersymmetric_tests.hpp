/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CIPHER_SYMMETRIC_TESTS_HPP
#define CIPHER_SYMMETRIC_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "cipher.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_SimpleString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 16;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeSymmetric);
		if (c == 0) {
			result = 1;
		} else if (c->init()) {
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
			const char * res = (char *) dec.buffer();
			if (strcmp(res, str)) {
				printf("%s != %s\n", res, str);
				result = 2;
			}
		}

		c->deinit();
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
		} else if (c->init()) {
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
			const char * res = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
			if (strlen(res) != str.length()) {
				result = 3;
			} else if (str.compareString(res)) {
				printf("%s != %s\n", res, str);
				result = 2;
			}
		}

		c->deinit();
		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void ciphersymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;
	
	LAUNCH_TEST(test_SimpleString, p, f);
	LAUNCH_TEST(test_LongString, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_SYMMETRIC_TESTS_HPP

