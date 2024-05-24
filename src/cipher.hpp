/**
 * author: brando
 * date: 5/24/24
 */

#ifndef CIPHER_HPP
#define CIPHER_HPP

#include <bflibcpp/object.hpp>

class Cipher : public BF::Object {
public:
	Cipher(const void * data, size_t size);
	~Cipher();
};

#endif // CIPHER_HPP

