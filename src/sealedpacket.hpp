/**
 * author: brando
 * date: 5/24/24
 */

#ifndef SEALED_PACKET_HPP
#define SEALED_PACKET_HPP

#include <bflibcpp/object.hpp>

/**
 * handles encryption and serialization of the
 * data
 *
 * TODO: the data here will be encrypted with a session key that
 * should get determined once the server creates the 
 * service
 */
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

