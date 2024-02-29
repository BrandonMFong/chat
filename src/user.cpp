/**
 * author: brando
 * date: 2/13/24
 */

#include "user.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<User *> currentuser;
Atomic<List<User * >> users;

void _UserRelease(User * user) {
	BFRelease(user);
}

User * User::current() {
	return currentuser.get();
}

void User::setCurrent(User * user) {
	currentuser.set(user);
}

User::User() {
	this->_username[0] = '\0';
	BFStringGetRandomUUIDString(this->_uuid);
}

User::~User() {
}

User * User::create(const char * username) {
	User * user = new User;

	if (user) {
		strncpy(user->_username, username, sizeof(user->_username));

		users.lock();

		if (users.unsafeget().count() == 0) {
			users.unsafeget().setDeallocateCallback(_UserRelease);
		}

		users.unsafeget().add(user);
		users.unlock();
	}

	return user;
}

const char * User::username() {
	return this->_username;
}

const char * User::uuid() {
	return this->_uuid;
}

