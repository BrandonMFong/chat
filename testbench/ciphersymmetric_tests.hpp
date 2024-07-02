/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CIPHER_SYMMETRIC_TESTS_HPP
#define CIPHER_SYMMETRIC_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "ciphersymmetric.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

void ciphersymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_SYMMETRIC_TESTS_HPP

