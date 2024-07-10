/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CIPHER_ASYMMETRIC_TESTS_HPP
#define CIPHER_ASYMMETRIC_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "cipherasymmetric.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_AsymSimpleString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 2;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeAsymmetric);
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
			const char * res = (char *) dec.buffer();
			if (strcmp(res, str)) {
				printf("%s != %s\n", res, str);
				result = 2;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_AsymLongString() {
	UNIT_TEST_START;
	int result = 0;
	int max = 2 << 1;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeAsymmetric);
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
			String decstr(dec);
			if (decstr.length() != str.length()) {
				result = 3;
			} else if (decstr != str) {
				printf("%s != %s\n", decstr, str);
				result = 2;
			}
		}

		BFDelete(c);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

int test_HandingOffPublicKeyToEncrypt() {
	UNIT_TEST_START;
	int result = 0;
	int max = 1;

	while (!result && max--) {
		Data pub;
		Cipher * user1 = Cipher::create(kCipherTypeAsymmetric);
		if (user1 == 0) {
			result = 1;
		} else if (user1->genkey()) {
			result = 2;
		}

		if (!result) {
			CipherAsymmetric * c = (CipherAsymmetric *) user1;
			result = c->getPublicKey(pub);
		}

		// now this is where we should hand off the public key
		Cipher * user2 = NULL;
		if (!result) {
			user2 = Cipher::create(kCipherTypeAsymmetric);
			if (user2 == 0) {
				result = 3;
			}
		}

		if (!result) {
			CipherAsymmetric * c = (CipherAsymmetric *) user2;
			result = c->setPublicKey(pub);
		}

		const char * msg = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
		Data pt(strlen(msg)+1, (const unsigned char *) msg);
		Data enc;
		if (!result) {
			result = user2->encrypt(pt, enc);
		}

		Data dec;
		if (!result) {
			result = user1->decrypt(enc, dec);
		}

		if (!result) {
			String str(dec);
			if (str.length() != strlen(msg)) {
				result = 10;
			} else if (str.compareString(msg)) {
				result = 11;
			}
		}

		BFDelete(user1);
		BFDelete(user2);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void cipherasymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_AsymSimpleString, p, f);
	LAUNCH_TEST(test_AsymLongString, p, f);
	LAUNCH_TEST(test_HandingOffPublicKeyToEncrypt, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_ASYMMETRIC_TESTS_HPP

