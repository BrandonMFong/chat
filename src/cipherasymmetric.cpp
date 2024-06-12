/**
 * author: brando
 * date: 5/29/24
 *
 * https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_encrypt_init_ex.html
 * https://medium.com/@amit.kulkarni/encrypting-decrypting-a-file-using-openssl-evp-b26e0e4d28d4
 */

#include "cipherasymmetric.hpp"
#include "log.hpp"
#include <openssl/rsa.h>
#include <openssl/err.h>

CipherAsymmetric::CipherAsymmetric() : Cipher() {
	this->_evpkey = NULL;
}

CipherAsymmetric::~CipherAsymmetric() {
	if (this->_evpkey) {
		EVP_PKEY_free(this->_evpkey);
	}
}

// TODO: make the asymmetric keys
int CipherAsymmetric::init() {
	this->_evpkey = EVP_PKEY_new();
	if (!this->_evpkey) {
		LOG_DEBUG("evp pkey is null");
		return 1;
	}

	//EVP_PKEY_new_raw_private_key(EVP_PKEY_X25519, 
	return 0;
}

int CipherAsymmetric::encrypt() {
	return 0;
}

int CipherAsymmetric::decrypt() {
	return 0;
}

