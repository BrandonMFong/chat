/**
 * author: brando
 * date: 2/13/24
 */

#ifndef USER_HPP
#define USER_HPP

#include <bflibcpp/object.hpp>
#include <typepacket.h>

/**
 *
 * This class has its own memory management
 */
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
	 * creates user based on user info
	 *
	 * caller does not own memory
	 */
	static User * create(const PayloadUserInfo * ui);

	/**
	 * gets current user that was assigned at app launch (see
	 * interface.cpp)
	 *
	 * returns null if no current user was set yet
	 */
	static const User * current();

	/**
	 * sets the current user
	 */
	static void setCurrent(User * user);

	virtual ~User();
	const char * username() const;
	void getuuid(uuid_t uuid) const;

	void getuserinfo(PayloadUserInfo * ui) const;

private:
	User(const char * username, const uuid_t uuid);
	char _username[USER_NAME_SIZE];
	uuid_t _uuid;
};

#endif // USER_HPP

