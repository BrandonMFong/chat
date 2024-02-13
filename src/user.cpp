/**
 * author: brando
 * date: 2/13/24
 */

#include "user.hpp"
#include <string.h>

User * _currentuser = 0;

User * User::current() {
	return _currentuser;
}

User::User() {
	this->_username[0] = '\0';

	if (_currentuser == NULL)
		_currentuser = this;
}

User::~User() {
	_currentuser = NULL;
}

void User::setUsername(const char * username) {
	strncpy(this->_username, username, sizeof(this->_username));
}

const char * User::username() {
	return this->_username;
}

