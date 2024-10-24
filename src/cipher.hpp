/**
 * author: brando
 * date: 5/29/24
 */

#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <bflibcpp/object.hpp>
#include <bflibcpp/data.hpp>

typedef enum {
kCipherTypeSymmetric = 0,
kCipherTypeAsymmetric = 1,
} CipherType;

class Cipher : public BF::Object {
public:
	static Cipher * create(CipherType type);
	~Cipher();

	virtual int genkey() = 0;
	virtual int encrypt(BF::Data & in, BF::Data & out) const = 0;
	virtual int decrypt(BF::Data & in, BF::Data & out) const = 0;

	virtual CipherType type() = 0;

	virtual bool isReady() const = 0;
protected:	
	Cipher();
};

#endif // CIPHER_HPP

