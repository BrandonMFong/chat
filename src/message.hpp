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
	Message(const Packet * pkt);
	virtual ~Message();

	const char * username() const;
	const char * data() const;
	const char * chatuuid() const;
private:
	char _data[DATA_BUFFER_SIZE];
	char _sendername[USER_NAME_SIZE];
	char _chatroomuuid[kBFStringUUIDStringLength];
};

#endif // MESSAGE_HPP

