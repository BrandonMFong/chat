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
	/**
	 * creates a new user
	 *
	 * caller does not own  memory. This class has its own memory
	 * management
	 */
	static User * create(const char * username);

	/**
	 * gets current user that was assigned at app launch (see
	 * interface.cpp)
	 *
	 * returns null if no current user was set yet
	 */
	static User * current();

	/**
	 * sets the current user
	 */
	static void setCurrent(User * user);

	virtual ~User();
	const char * username();
	const char * uuid();

private:
	User();
	char _username[USER_NAME_SIZE];
	char _uuid[kBFStringUUIDStringLength];
};

#endif // USER_HPP

