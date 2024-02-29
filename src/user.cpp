/**
 * author: brando
 * date: 2/13/24
 */

#include "user.hpp"
#include "log.hpp"
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

User::User(const char * username, const uuid_t uuid) {
	strncpy(this->_username, username, sizeof(this->_username));
	uuid_copy(this->_uuid, uuid);
}

User::~User() {
}

void _UserAddUserToUsers(User * user) {
	users.lock();

	if (users.unsafeget().count() == 0) {
		users.unsafeget().setDeallocateCallback(_UserRelease);
	}

	users.unsafeget().add(user);
	users.unlock();
}

User * User::create(const char * username) {
	if (!username)
		return NULL;

	uuid_t uuid;
	uuid_generate_random(uuid);
	
	User * user = new User(
		username,
		uuid
	);
	_UserAddUserToUsers(user);

	return user;
}

User * User::create(const PayloadUserInfo * ui) {
	if (!ui) 
		return NULL;

	User * user = new User(
		ui->username,
		ui->useruuid
	);
	_UserAddUserToUsers(user);

	return user;
}

const char * User::username() const {
	return this->_username;
}

void User::getuuid(uuid_t uuid) const {
	uuid_copy(uuid, this->_uuid);
}

void User::getuserinfo(PayloadUserInfo * ui) const {
	if (!ui) return;

	LOG_DEBUG("filling in user info");

	strncpy(ui->username, this->_username, sizeof(this->_username));
	this->getuuid(ui->useruuid);
}

