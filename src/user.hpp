/**
 * author: brando
 * date: 2/13/24
 */

#ifndef USER_HPP
#define USER_HPP

#include <bflibcpp/object.hpp>
#include "typepacket.h"

class Cipher;

/**
 *
 * This class has its own memory management
 */
class User : public BF::Object {
public:
	static User * getuser(const uuid_t uuid);

	/**
	 * caller owns memory
	 */
	static PayloadUserInfo ** getUserList(int * size, int * err);

	/**
	 * creates a new user
	 *
	 * retain count on return is +2. Caller is responsible for releasing object after they are done
	 */
	static User * create(const char * username);

	/**
	 * creates user based on user info
	 *
	 * caller does not own memory
	 */
	static User * create(const PayloadUserInfo * ui);

	/**
	 * releases memory and removes from internal list
	 */
	static int destroy(User * user);
	
	virtual ~User();
	const char * username() const;
	void getuuid(uuid_t uuid) const;

	void getuserinfo(PayloadUserInfo * ui) const;

private:
	User(const char * username, const uuid_t uuid);
	char _username[USER_NAME_SIZE];
	uuid_t _uuid;

	int initCipher();
	Cipher * _cipher;
};

#endif // USER_HPP

