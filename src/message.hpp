/**
 * author: brando
 * date: 2/23/24
 */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <bflibcpp/object.hpp>
#include "typepacket.h"

class Cipher;

class Message : public BF::Object {
public:
	/**
	 * pkt : copies memory
	 */
	Message(const Packet * pkt);
	virtual ~Message();

	const char * username() const;
	const char * data() const;
	void getuuidchatroom(uuid_t uuid) const;
	void getuuiduser(uuid_t uuid) const;
	PayloadMessageType type() const;

	/**
	 * returns the current state of the packet
	 */
	const Packet * packet() const;

	/**
	 * Note about decryptData and encryptData:
	 *
	 * Message does not keep track if data is 
	 * encrypted or not. It is up the the caller
	 * to determine the state of the data
	 *
	 * Each function checks Cipher::isReady()
	 */

	int decryptData(const Cipher * cipher);
	int encryptData(const Cipher * cipher);

private:
	
	/**
	 * this object is basically a wrapper around a
	 * message packet
	 */
	Packet _packet;
};

#endif // MESSAGE_HPP

