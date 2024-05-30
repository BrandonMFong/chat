/**
 * author: brando
 * date: 5/29/24
 */

#include "ciphersymmetric.hpp"
#include "log.hpp"
#include <openssl/rand.h>

CipherSymmetric::CipherSymmetric() : Cipher() {

}

CipherSymmetric::~CipherSymmetric() {

}

int CipherSymmetric::init() {
	if (RAND_bytes(this->_key, CIPHER_SYMMETRIC_KEY_SIZE) != 1) {
		LOG_DEBUG("couldn't generate key");
		return 1;
	}

	return 0;
}

