/**
 * author: brando
 * date: 2/23/24
 */

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <bflibcpp/object.hpp>
#include "typepacket.h"

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
private:
	
	/**
	 * this object is basically a wrapper around a
	 * message packet
	 */
	Packet _packet;
};

#endif // MESSAGE_HPP

