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

/**
 * main functionality is a display window and prompt window
 */
class Interface : public BF::Object {
	friend class Chatroom;
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
	int windowCreateModeCreateChatroom();
	static void displayWindowUpdateThread(void * in);

	/**
	 * updates UI based on current state
	 */
	int draw();

	/**
	 * process user input
	 */
	int processinput(InputBuffer & buf);

	void chatroomListHasChanged();
	void converstaionHasChanged();
	
	BFLock _winlock;
	WINDOW * _inputWin;
	WINDOW * _displayWin;
	WINDOW * _helpWin;
	Chatroom * _chatroom;
	BF::Atomic<User *> _user;

	BF::Atomic<InterfaceState> _state;
	BF::Atomic<InterfaceState> _prevstate;
	BF::Atomic<bool> _updatechatroomlist;
	BF::Atomic<bool> _updateconversation;
};

#endif // INTERFACE_HPP

