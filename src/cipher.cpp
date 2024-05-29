/**
 * author: brando
 * date: 5/29/24
 */

#include "cipher.hpp"
#include "cipherasymmetric.hpp"
#include "ciphersymmetric.hpp"

Cipher::Cipher() {

}

Cipher::~Cipher() {

}

Cipher * Cipher::create(CipherType type) {
	Cipher * result = NULL;

	switch (type) {
	case kCipherTypeSymmetric:
		result = new CipherSymmetric;
		break;
	case kCipherTypeAsymmetric:
		result = new CipherAsymmetric;
		break;
	}

	return result;
}

