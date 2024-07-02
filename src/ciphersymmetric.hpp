/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_SYMMETRIC_HPP
#define CIPHER_SYMMETRIC_HPP

#include "cipher.hpp"

/// in bytes
#define CIPHER_SYMMETRIC_KEY_SIZE 32
#define CIPHER_SYMMETRIC_IV_SIZE 16

/**
 * will follow https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
 */
class CipherSymmetric : public Cipher {
	friend class Cipher;
public:
	~CipherSymmetric();

	/**
	 * generates symm key
	 */
	int init();
	int encrypt(BF::Data & in, BF::Data & out);
	int decrypt();

private:
	CipherSymmetric();

	unsigned char _key[CIPHER_SYMMETRIC_KEY_SIZE];
	unsigned char _iv[CIPHER_SYMMETRIC_IV_SIZE];
};

#endif // CIPHER_SYMMETRIC_HPP

