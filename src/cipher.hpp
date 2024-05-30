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

	virtual int generate() = 0;
protected:	
	Cipher();
};

#endif // CIPHER_HPP

