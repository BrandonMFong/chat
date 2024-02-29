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
	static User * create(const char * username);
	static User * current();
	virtual ~User();
	const char * username();
	const char * uuid();
private:
	User();
	char _username[USER_NAME_SIZE];
	char _uuid[kBFStringUUIDStringLength];
};

#endif // USER_HPP

