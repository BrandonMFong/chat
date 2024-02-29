/**
 * author: brando
 * date: 2/13/24
 */

#include "user.hpp"
#include <string.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

User * currentuser = 0;
Atomic<List<User * >> users;

User * User::current() {
	return currentuser;
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
	}

	return user;
}

const char * User::username() {
	return this->_username;
}

const char * User::uuid() {
	return this->_uuid;
}

