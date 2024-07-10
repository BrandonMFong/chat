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
	int max = 1;

	while (!result && max--) {
		Cipher * c = Cipher::create(kCipherTypeAsymmetric);
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


void cipherasymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_AsymSimpleString, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_ASYMMETRIC_TESTS_HPP

