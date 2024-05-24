/**
 * author: brando
 * date: 5/24/24
 *
 * assists in encryption
 */

#ifndef SEALED_PACKET_HPP
#define SEALED_PACKET_HPP

#include <bflibcpp/object.hpp>

class SealedPacket : public BF::Object {
public:
	SealedPacket(const void * data, size_t size);
	~SealedPacket();

	bool isEncrypted();
	const void * data();
	size_t size();

private:
	void * _dataPlain;
	size_t _dataPlainSize;
};

#endif // SEALED_PACKET_HPP

