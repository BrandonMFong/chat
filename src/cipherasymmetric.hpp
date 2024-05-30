/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_ASYMMETRIC_HPP
#define CIPHER_ASYMMETRIC_HPP

#include "cipher.hpp"

class CipherAsymmetric : public Cipher {
	friend class Cipher;
public:
	~CipherAsymmetric();
	int generate();

private:
	CipherAsymmetric();
};

#endif // CIPHER_ASYMMETRIC_HPP

