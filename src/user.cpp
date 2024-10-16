/**
 * author: brando
 * date: 2/13/24
 *
 * https://medium.com/@amit.kulkarni/encrypting-decrypting-a-file-using-openssl-evp-b26e0e4d28d4
 */

#include "user.hpp"
#include "log.hpp"
#include "interface.hpp"
#include "cipher.hpp"
#include <string.h>
#include <unistd.h>
#include <bflibcpp/bflibcpp.hpp>

using namespace BF;

Atomic<List<User * >> users;

User * User::getuser(const uuid_t uuid) {
	users.lock();
	User * result = NULL;
	List<User *>::Node * n = users.unsafeget().first();
	for (; n; n = n->next()) {
		result = n->object();
		uuid_t u;
		result->getuuid(u);
		if (!uuid_compare(u, uuid))
			break;
		result = NULL;
	}
	users.unlock();
	return result;
}

void _UserRelease(User * user) {
	BFRelease(user);
}

User::User(const char * username, const uuid_t uuid) {
	strncpy(this->_username, username, sizeof(this->_username));
	uuid_copy(this->_uuid, uuid);
	this->_cipher = NULL;
}

User::~User() {
	BFDelete(this->_cipher);
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

PayloadUserInfo ** User::getUserList(int * size, int * err) {
	if (!size)
		return NULL;

	users.lock();
	*size = users.unsafeget().count();
	PayloadUserInfo ** result = (PayloadUserInfo **) malloc(
		sizeof(PayloadUserInfo *) * (*size)
	);
	List<User *>::Node * n = users.unsafeget().first();
	for (int i = 0; n; n = n->next()) {
		User * u = n->object();
		if (u) {
			result[i] = (PayloadUserInfo *) malloc(
				sizeof(PayloadUserInfo)
			);

			u->getuserinfo(result[i++]);
		}
	}

	users.unlock();
	return result;
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

	if (user->initCipher()) {
		LOG_DEBUG("couldn't init cipher");
	}

	Interface::current()->userListHasChanged();

	return user;
}

int User::initCipher() {
	this->_cipher = Cipher::create(kCipherTypeAsymmetric);
	if (!this->_cipher) {
		LOG_DEBUG("cipher is null");
		return 1;
	}

	if (this->_cipher->genkey()) {
		LOG_DEBUG("couldn't initiate cipher");
		return 1;
	}

	return 0;
}

User * User::create(const PayloadUserInfo * ui) {
	if (!ui) 
		return NULL;

	User * user = new User(
		ui->username,
		ui->useruuid
	);
	_UserAddUserToUsers(user);

	Interface::current()->userListHasChanged();

	return user;
}

int User::destroy(User * user) {
	if (user) {
		users.get().pluckObject(user);
		BFRelease(user);
		Interface::current()->userListHasChanged();
		return 0;
	}
	return 1;
}

const Cipher * User::cipher() {
	return this->_cipher;
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

