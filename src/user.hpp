/**
 * author: brando
 * date: 2/13/24
 */

#ifndef USER_HPP
#define USER_HPP

#include <bflibcpp/object.hpp>
#include <typepacket.h>

class User : public BF::Object {
public:
	static User * current();
	User();
	virtual ~User();
	void setUsername(const char * username);
	const char * username();
	const char * uuid();
private:
	char _username[USER_NAME_SIZE];
	char _uuid[kBFStringUUIDStringLength];
};

#endif // USER_HPP

