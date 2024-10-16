/**
 * author: brando
 * date: 1/25/24
 */

#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include <bfnet/socket.hpp>
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

	int windowCreateStateDraft();
	int windowCreateStateDraft(int inputWinWidth, int inputWinHeight);
	int windowCreateModeHelp();
	int windowCreateStateChatroom();
	int windowCreateModeLobby();
	
	static void displayWindowUpdateThread(void * in);

	int processinputStateDraft(InputBuffer & userInput);
	int processinputStateChatroom(InputBuffer & userInput);
	int processinputStateLobby(InputBuffer & userInput);

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

	/**
	 * reads user input from modal window
	 */
	int readUserInput(InputBuffer & userInput);

	// trigger events
	void userListHasChanged();
	void chatroomListHasChanged();	
	void converstaionHasChanged();
	void lobbyHasChanged();

	/**
	 * this will halt the workloop that drives the interface
	 * updates. This should be released when something has changed
	 * that requires a ui redraw
	 */
	BFLock _uistoplight;

	void setErrorMessage(BF::String errmsg);
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

