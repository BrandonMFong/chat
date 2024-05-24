/**
 * author: brando
 * date: 5/24/24
 *
 * assists in encryption
 */

#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <bflibcpp/object.hpp>

class Cipher : public BF::Object {
public:
	Cipher(const void * data, size_t size);
	~Cipher();

	bool isEncrypted();
	const void * data();
	size_t size();

private:
	void * _dataPlain;
	size_t _dataPlainSize;
};

#endif // CIPHER_HPP

