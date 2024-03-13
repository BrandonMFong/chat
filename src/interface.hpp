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
#include <bflibcpp/string.hpp>
#include <ncurses.h>

#define INTERFACE_COMMAND_DRAFT "draft"
#define INTERFACE_COMMAND_QUIT "quit"
#define INTERFACE_COMMAND_CREATE "create"
#define INTERFACE_COMMAND_JOIN "join"
#define INTERFACE_COMMAND_LEAVE "leave"
#define INTERFACE_COMMAND_HELP "help"

class User;
class InputBuffer;
class Chatroom;

/**
 * main functionality is a display window and prompt window
 */
class Interface : public BF::Object {
	friend class ChatroomServer;
	friend class ChatroomClient;
	friend class Chatroom;
	friend class User;
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

	InterfaceState currstate();
	InterfaceState prevstate();

protected:
	Interface();

	/**
	 * updates UI based on current state
	 */
	virtual int draw();

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
	 * draws lobby ui
	 *
	 * boxes, lines, etc.
	 */
	int drawDisplayWindowLobby();

	// lobby content
	int windowWriteContentLobby();
	int windowWriteChatroomList();
	int windowWriteUserList();

	// chatroom content
	int windowWriteConversation();

	/**
	 * process user input
	 */
	int processinput(InputBuffer & buf);

	// trigger events
	void userListHasChanged();
	void chatroomListHasChanged();
	void converstaionHasChanged();

	void throwErrorMessage(BF::String errmsg);
	BF::String _errorMessage;

	// might be wise to ensure we lock this 
	// before locking other objects
	BFLock _winlock;

	WINDOW * _inputWin;
	WINDOW * _displayWin;
	WINDOW * _headerWin;
	WINDOW * _helpWin;

	BF::Atomic<Chatroom *> _chatroom;
	BF::Atomic<User *> _user;

	BF::Atomic<InterfaceState> _state;
	BF::Atomic<InterfaceState> _prevstate;
	InterfaceState _returnfromhelpstate;
	BF::Atomic<bool> _updatelobby;
	BF::Atomic<bool> _updateconversation;
};

#endif // INTERFACE_HPP

