/**
 * author: brando
 * date: 2/13/24
 */

#include "user.hpp"
#include <string.h>
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<User *> currentuser;
Atomic<List<User * >> users;

void _UserRelease(User * user) {
	BFRelease(user);
}

const User * User::current() {
	while (!currentuser.get()) {
		// current user is stil null
		// 
		// we will wait for current
		// user to be set before returning
		usleep(50);
	}

	// should we return current user as an 
	// atomic object?
	//
	// I believe we should revisit this if
	// we are intending to modify user
	// outside of this
	//
	// for now we are going to return curr
	// user as const
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

const char * User::username() const {
	return this->_username;
}

const char * User::uuid() const {
	return this->_uuid;
}

void User::getuserinfo(PayloadUserInfo * ui) const {

}

