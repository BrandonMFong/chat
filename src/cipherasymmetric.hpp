/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_ASYMMETRIC_HPP
#define CIPHER_ASYMMETRIC_HPP

#include "cipher.hpp"
#include <openssl/evp.h>

/// in bytes
#define CIPHER_ASYMMETRIC_KEY_SIZE 32

class CipherAsymmetric : public Cipher {
	friend class Cipher;
public:
	~CipherAsymmetric();
	int init();
	int encrypt();
	int decrypt();

private:
	CipherAsymmetric();

	/**
	 * https://www.openssl.org/docs/man3.0/man3/EVP_PKEY_new.html
	 */
	EVP_PKEY * _evpkey;
};

#endif // CIPHER_ASYMMETRIC_HPP

