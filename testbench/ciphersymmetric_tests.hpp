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

	Cipher * c = Cipher::create(kCipherTypeSymmetric);
	if (c == 0) {
		result = 1;
	}

	const char * str = "Hello world!";
	Data plain(strlen(str)+1, (unsigned char *) str);
	Data enc;
	if (!result) {
		result = c->encrypt(plain, enc);
	}

	BFDelete(c);

	UNIT_TEST_END(!result, result);
	return result;
}

void ciphersymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;
	
	LAUNCH_TEST(test_SimpleString, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_SYMMETRIC_TESTS_HPP

