/**
 * author: Brando
 * date: 2/20/24
 */

#ifndef CIPHER_SYMMETRIC_TESTS_HPP
#define CIPHER_SYMMETRIC_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "cipher.hpp"
#include "log.hpp"
#include "ciphersymmetric.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_SymCheckReady, 2<<2, {
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
			printf("%s != %s\n", res.cString(), str);
			result = 3;
		}
	}

	BFRelease(c);
})

BFTEST_UNIT_FUNC(test_SimpleString, 2<<10, {
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
			result = 3;
		}
	}

	BFRelease(c);
})

BFTEST_UNIT_FUNC(test_EmptyString, 2<<10, {
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
		if (res != str) {
			printf("%s != %s\n", res.cString(), str.cString());
			printf("%ld != %ld\n", res.length(), str.length());
			printf("\ndec len %ld\n", dec.size());
			result = 3;
		}
	}

	BFRelease(c);
})

BFTEST_UNIT_FUNC(test_LongString, 2<<10, {
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
			printf("%s != %s\n", res.cString(), str.cString());
			result = 3;
		}
	}

	BFRelease(c);
})

BFTEST_UNIT_FUNC(test_RandomBytes, 2<<10, {
	Cipher * c = Cipher::create(kCipherTypeSymmetric);
	if (c == 0) {
		result = 1;
	} else if (c->genkey()) {
		result = 2;
	}

	size_t size = PAYLOAD_MESSAGE_LIMIT_MESSAGE;
	unsigned char * buf = (unsigned char *) malloc(size);
	srand(time(0));
	for (int i = 0; i < size; i++) {
		buf[i] = rand() % (2 << 7);
	}
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
	BFRelease(c);
})

BFTEST_UNIT_FUNC(test_HandingOffKey, 2<<10, {
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
			printf("%s != %s\n", res.cString(), str.cString());
			result = 2;
		}
	}

	BFRelease(u1);
	BFRelease(u2);
})

BFTEST_COVERAGE_FUNC(ciphersymmetric_tests, {
	BFTEST_LAUNCH(test_SimpleString);
	BFTEST_LAUNCH(test_HandingOffKey);
	BFTEST_LAUNCH(test_RandomBytes);
	BFTEST_LAUNCH(test_LongString);
	BFTEST_LAUNCH(test_EmptyString);
	BFTEST_LAUNCH(test_SymCheckReady);
})

#endif // CIPHER_SYMMETRIC_TESTS_HPP

