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

Atomic<List<User * >> users;

void _UserRelease(User * user) {
	BFRelease(user);
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
		users.unsafeget().setReleaseCallback(_UserRelease);
	}

	BFRetain(user);
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

	strncpy(ui->username, this->_username, sizeof(this->_username));
	this->getuuid(ui->useruuid);
}

