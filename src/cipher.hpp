/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <bflibcpp/object.hpp>

typedef enum {
kCipherTypeSymmetric = 0,
kCipherTypeAsymmetric = 1,
} CipherType;

class Cipher : public BF::Object {
public:
	static Cipher * create(CipherType type);
	~Cipher();

	virtual int init() = 0;
	virtual int encrypt() = 0;
	virtual int decrypt() = 0;
protected:	
	Cipher();
};

#endif // CIPHER_HPP

