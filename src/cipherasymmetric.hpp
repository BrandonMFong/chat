/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_ASYMMETRIC_HPP
#define CIPHER_ASYMMETRIC_HPP

#include "cipher.hpp"

#include <openssl/core_names.h>
#include <openssl/evp.h>
#include <openssl/err.h>

/// in bytes
#define CIPHER_ASYMMETRIC_KEY_SIZE 32

class CipherAsymmetric : public Cipher {
	friend class Cipher;
public:
	~CipherAsymmetric();
	int genkey();
	int encrypt(BF::Data & in, BF::Data & out) const;
	int decrypt(BF::Data & in, BF::Data & out) const;

	CipherType type();

	int getPublicKey(BF::Data & key) const;
	int setPublicKey(BF::Data & key);
	int getPrivateKey(BF::Data & key) const;
private:
	CipherAsymmetric();
	EVP_PKEY * _keys;
    /*
     * Setting libctx to NULL uses the default library context
     * Use OSSL_LIB_CTX_new() to create a non default library context
     */
    OSSL_LIB_CTX * _libctx;
};

#endif // CIPHER_ASYMMETRIC_HPP

