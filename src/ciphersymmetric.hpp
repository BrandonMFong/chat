/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_SYMMETRIC_HPP
#define CIPHER_SYMMETRIC_HPP

#include "cipher.hpp"

/// in bytes
#define CIPHER_SYMMETRIC_KEY_SIZE 16

class CipherSymmetric : public Cipher {
	friend class Cipher;
public:
	~CipherSymmetric();
	int generate();

private:
	CipherSymmetric();

	unsigned char _key[CIPHER_SYMMETRIC_KEY_SIZE];
};

#endif // CIPHER_SYMMETRIC_HPP

