/**
 * author: brando
 * date: 1/25/24
 */

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "socket.hpp"
#include "typepacket.h"
#include "typeinterfacestate.hpp"
#include <bflibcpp/object.hpp>
#include <bflibcpp/atomic.hpp>
#include <ncurses.h>

class User;
class InputBuffer;
class Chatroom;

class Interface : public BF::Object {
public:
	static Interface * create(char mode);

	/**
	 * returns current interface
	 */
	static Interface * current();

	virtual ~Interface();
	int run();

	/**
	 * current user on this machine
	 */
	User * getuser();

protected:
	Interface();

private:
	int windowStart();
	int windowLoop();
	int windowStop();
	int gatherUserData();
	int windowUpdateInputWindowText(InputBuffer & userInput);
	int windowCreateModeEdit();
	int windowCreateModeHelp();
	int windowCreateModeCommand();
	int windowCreateModeLobby();
	static void displayWindowUpdateThread(void * in);

	int draw();
	int processinput(InputBuffer & buf);
	
	BFLock _winlock;
	WINDOW * _inputWin;
	WINDOW * _displayWin;
	WINDOW * _helpWin;
	Chatroom * _chatroom;
	BF::Atomic<User *> _user;

	InterfaceState _state;
	InterfaceState _prevstate;
};

#endif // INTERFACE_HPP

