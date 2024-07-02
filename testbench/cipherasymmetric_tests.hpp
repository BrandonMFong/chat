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

void cipherasymmetric_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // CIPHER_ASYMMETRIC_TESTS_HPP

