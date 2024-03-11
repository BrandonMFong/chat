/**
 * author: brando
 * date: 1/25/24
 */

#include "interface.hpp"
#include "interfaceserver.hpp"
#include "interfaceclient.hpp"
#include "chat.hpp"
#include "user.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>
#include <string.h>
#include "log.hpp"
#include "inputbuffer.hpp"
#include "office.hpp"
#include "chatroom.hpp"
#include "chatroomserver.hpp"
#include "message.hpp"
#include "agentclient.hpp"
#include "command.hpp"

using namespace BF;

const size_t linelen = DATA_BUFFER_SIZE + USER_NAME_SIZE + (2 << 4);
Interface * interface = NULL;

Interface * Interface::current() {
	return interface;
}

Interface::Interface() {
	interface = this;
	this->_inputWin = NULL;
	this->_displayWin = NULL;
	this->_helpWin = NULL;
	this->_chatroom = NULL;
	this->_state = kInterfaceStateUnknown;
	this->_prevstate = kInterfaceStateUnknown;
	this->_updatechatroomlist = false;
	this->_updateconversation = false;
	this->_user = NULL;

	BFLockCreate(&this->_winlock);
}

Interface::~Interface() {
	BFLockDestroy(&this->_winlock);
	Chatroom * c = this->_chatroom;
	BFRelease(c);
}

Interface * Interface::create(char mode) {
	switch (mode) {
	case SOCKET_MODE_SERVER:
		return new InterfaceServer;
	case SOCKET_MODE_CLIENT:
		return new InterfaceClient;
	default:
		return NULL;
	}
}

void Interface::chatroomListHasChanged() {
	this->_updatechatroomlist = true;
}

void Interface::converstaionHasChanged() {
	this->_updateconversation = true;
}

InterfaceState Interface::currstate() {
	return this->_state.get();
}

InterfaceState Interface::prevstate() {
	return this->_prevstate.get();
}

int InterfaceCraftChatLineFromMessage(const Message * msg, char * line) {
	if (!msg || !line) return 30;

	switch (msg->type()) {
	case kPayloadMessageTypeData:
		snprintf(line, linelen, "%s> %s", msg->username(), msg->data());
		return 0;
	case kPayloadMessageTypeUserJoined:
		char username[USER_NAME_SIZE];
		uuid_t u0, u1;
		Interface::current()->getuser()->getuuid(u0);
		msg->getuuiduser(u1);
		if (!uuid_compare(u0, u1)) {
			strncpy(username, "You", USER_NAME_SIZE);
		} else {
			strncpy(username, msg->username(), USER_NAME_SIZE);
		}

		snprintf(line, linelen, "<<%s joined the chat>>", username, msg->data());
		return 0;
	default:
		return 31;
	}
}

void _InterfaceDrawMessage(WINDOW * dispwin, int & row, int col, const Message * m) {
	if (m && dispwin) {
		char line[linelen];
		if (!InterfaceCraftChatLineFromMessage(m, line)) {
			mvwprintw(dispwin, (row++) + 1, 1, line);
		}
	}
}

int Interface::windowWriteConversation() {
	this->_updateconversation.lock();
	this->_chatroom.lock();
	if (this->_updateconversation.unsafeget() && this->_chatroom.unsafeget()) {
		this->_chatroom.unsafeget()->conversation.lock();
		BFLockLock(&this->_winlock);

		werase(this->_displayWin);
		box(this->_displayWin, 0, 0);

		// write messages
		int row = 0;
		for (int i = 0; i < this->_chatroom.unsafeget()->conversation.unsafeget().count(); i++) {
			Message * m = this->_chatroom.unsafeget()->conversation.unsafeget().objectAtIndex(i);
			_InterfaceDrawMessage(this->_displayWin, row, 1, m);
		}

		wrefresh(this->_displayWin);
		
		this->_updateconversation.unsafeset(false);

		BFLockUnlock(&this->_winlock);
		this->_chatroom.unsafeget()->conversation.unlock();
	}
	this->_chatroom.unlock();
	this->_updateconversation.unlock();
	return 0;
}

int Interface::windowWriteChatroomList() {
	this->_updatechatroomlist.lock();
	if (this->_updatechatroomlist.unsafeget()) {
		BFLockLock(&this->_winlock);
		werase(this->_displayWin);
		box(this->_displayWin, 0, 0);

		// print title
		int row = 1;
		mvwprintw(this->_displayWin, row++, 1, "Chatrooms:");

		// get list
		int size = 0;
		int error = 0;
		PayloadChatInfo ** list = Chatroom::getChatroomList(&size, &error);
		if (!list || error) {
			LOG_DEBUG("could not get list of chatrooms: %d", error);
		} else {

			// show available rooms
			for (int i = 0; i < size; i++) { 
				char line[512];
				snprintf(line, 512, "(%d) \"%s\"", i+1, list[i]->chatroomname);
				mvwprintw(this->_displayWin, row++, 1, line);

				BFFree(list[i]);
			}
			BFFree(list);
		}

		wrefresh(this->_displayWin);
		this->_updatechatroomlist.unsafeset(false);
		BFLockUnlock(&this->_winlock);
	}
	this->_updatechatroomlist.unlock();

	return 0;
}

void Interface::displayWindowUpdateThread(void * in) {
	int error = 0;
	Interface * interface = (Interface *) in;
	const BFThreadAsyncID tid = BFThreadAsyncGetID();

	while (!BFThreadAsyncIsCanceled(tid)) {
		switch (interface->_state.get()) 
		{
			case kInterfaceStateLobby:
			{
				interface->windowWriteChatroomList();
				break;
			}

			// allow the display window to always update its conversation
			// even when we are typing
			case kInterfaceStateChatroom:
			case kInterfaceStateDraft: 
			{
				interface->windowWriteConversation();
				break;
			}
		}
	}
}

#define DELETE_WINDOWS \
	if (this->_inputWin) \
		delwin(this->_inputWin); \
	if (this->_displayWin) \
		delwin(this->_displayWin); \
	if (this->_helpWin) \
		delwin(this->_helpWin); \
	if (this->_headerWin) \
		delwin(this->_headerWin);


int Interface::windowCreateModeLobby() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;

	// Create two windows
	this->_headerWin = newwin(1, COLS, 0, 0);
	this->_displayWin = newwin(LINES - 2, COLS, 1, 0);
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);

	box(this->_displayWin, 0, 0); // Add a box around the display window

	char title[COLS];
	snprintf(title, COLS, "Lobby");
	int y = (COLS - strlen(title)) / 2;
	mvwprintw(this->_headerWin, 0, y, title);

	refresh();
	wrefresh(this->_inputWin);
	wrefresh(this->_displayWin);
	wrefresh(this->_headerWin);

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window
	
	this->_updatechatroomlist = true;

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateModeCommand() {
	// change to normal mode
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;
	
	// Create two windows
	this->_headerWin = newwin(1, COLS, 0, 0);
	this->_displayWin = newwin(LINES - 2, COLS, 1, 0);
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);

	box(this->_displayWin, 0, 0); // Add a box around the display window

	char title[COLS];
	snprintf(title, COLS, "%s", this->_chatroom.get()->name());
	int y = (COLS - strlen(title)) / 2;
	mvwprintw(this->_headerWin, 0, y, title);

	refresh();
	wrefresh(this->_inputWin);
	wrefresh(this->_displayWin);
	wrefresh(this->_headerWin);

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window

	this->_updateconversation = true;

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateModeEdit() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;
	
	// Create two windows
	this->_headerWin = newwin(1, COLS, 0, 0);
	this->_displayWin = newwin(LINES - 4, COLS, 1, 0);
	this->_inputWin = newwin(3, COLS, LINES - 3, 0);

	box(this->_inputWin, 0, 0); // Add a box around the input window
	box(this->_displayWin, 0, 0); // Add a box around the display window

	char title[COLS];
	snprintf(title, COLS, "%s - draft", this->_chatroom.get()->name());
	int y = (COLS - strlen(title)) / 2;
	mvwprintw(this->_headerWin, 0, y, title);

	refresh();
	wrefresh(this->_inputWin);
	wrefresh(this->_displayWin);
	wrefresh(this->_headerWin);

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window
	
	this->_updateconversation = true;

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateModeHelp() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;

	this->_helpWin = newwin(LINES - 10, COLS - 10, 5, 5);
	box(this->_helpWin, 0, 0); // Draw a box around the sub-window

	// dialog
	mvwprintw(this->_helpWin, 1, 3, " '%s' : Draft message. To send hit enter key.", INTERFACE_COMMAND_DRAFT);
	mvwprintw(this->_helpWin, 2, 3, " '%s' : Quits application.", INTERFACE_COMMAND_QUIT);
	mvwprintw(this->_helpWin, 3, 3, " '%s' [ <name> ] : Creates chatroom with <name>.", INTERFACE_COMMAND_CREATE);
	mvwprintw(this->_helpWin, 4, 3, " '%s' <index> : Joins chatroom at index.", INTERFACE_COMMAND_JOIN);
	mvwprintw(this->_helpWin, 5, 3, " '%s' : Leaves chatroom.", INTERFACE_COMMAND_LEAVE);
	mvwprintw(this->_helpWin, LINES - 12, 3, "Press any key to close...");

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_helpWin); // Refresh the help window

	BFLockUnlock(&this->_winlock);
	return 0;
}

int Interface::windowUpdateInputWindowText(InputBuffer & userInput) {
	BFLockLock(&this->_winlock);
	switch (this->_state.get()) {
	case kInterfaceStateChatroom:
	case kInterfaceStateLobby:
		werase(this->_inputWin);
		mvwprintw(this->_inputWin, 0, 0, userInput.cString());
		wmove(this->_inputWin, 0, userInput.cursorPosition());
		wrefresh(this->_inputWin);
		break;
	case kInterfaceStateDraft:
		werase(this->_inputWin);
		box(this->_inputWin, 0, 0); // Add a box around the display window
		mvwprintw(this->_inputWin, 1, 1, userInput.cString());
		wmove(this->_inputWin, 1, userInput.cursorPosition() + 1);
		wrefresh(this->_inputWin);
		break;
	}
	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowStart() {
	initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me
    noecho();  // Don't echo user input

	return 0;
}

int Interface::windowStop() {
	DELETE_WINDOWS;
    endwin(); // End curses mode

	return 0;
}

int Interface::draw() {
	// only create new windows if
	// we changed states
	if (this->_state != this->_prevstate) {
		switch (this->_state.get()) {
		case kInterfaceStateLobby:
			this->windowCreateModeLobby();
			break;
		case kInterfaceStateChatroom:
			this->windowCreateModeCommand();
			break;
		case kInterfaceStateDraft:
			this->windowCreateModeEdit();
			break;
		case kInterfaceStateHelp:
			this->windowCreateModeHelp();
			break;
		}
	}

	return 0;
}

Chatroom * _InterfaceGetChatroomAtIndex(int i) {
	Chatroom * res = NULL;

	// get list
	int size = 0;
	int error = 0;
	PayloadChatInfo ** list = Chatroom::getChatroomList(&size, &error);
	if (!list || error) {
		LOG_DEBUG("could not get list of chatrooms: %d", error);
	} else {
		res = Chatroom::getChatroom(list[i]->chatroomuuid);

		for (int i = 0; i < size; i++) { 
			BFFree(list[i]);
		}
		BFFree(list);
	}

	return res;
}

int Interface::processinput(InputBuffer & userInput) {
	switch (this->_state.get()) {
	case kInterfaceStateLobby:
		if (userInput.isready()) {
			Command cmd(userInput);
			if (!cmd.op().compareString(INTERFACE_COMMAND_QUIT)) { // quit
				this->_state = kInterfaceStateQuit;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) { // help
				this->_returnfromhelpstate = this->_state;
				this->_state = kInterfaceStateHelp;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_CREATE)) { // create
				char chatroomname[CHAT_ROOM_NAME_SIZE];
				if (cmd.count() > 1) {
					strncpy(chatroomname, cmd[1], CHAT_ROOM_NAME_SIZE);
				} else {
					// set up chat room name
					//
					// right now we are automatically creating a chatroom. The
					// user should be able to customize the room name
					snprintf(chatroomname, CHAT_ROOM_NAME_SIZE, "chatroom%d",
							Chatroom::getChatroomsCount());
				}

				Chatroom * cr = ChatroomServer::create(chatroomname);
				BFRelease(cr);
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_JOIN)) { // join
				int index = String::toi(cmd[1]) - 1;
				if ((index >= 0) && (index < Chatroom::getChatroomsCount())) {
					this->_chatroom = _InterfaceGetChatroomAtIndex(index);
					if (this->_chatroom) {
						Object::retain(this->_chatroom.get());

						// enrolls current user on this machine to the chatroom
						this->_chatroom.get()->enroll(this->_user.get());

						this->_state = kInterfaceStateChatroom;
					}
				}
			}
			userInput.reset();
		}
		break;
	case kInterfaceStateChatroom:
		if (userInput.isready()) { 
			Command cmd(userInput);
			if (!cmd.op().compareString(INTERFACE_COMMAND_LEAVE)) { // leave
				LOG_DEBUG("leaving chat");
				// tell chat room we are leaving
				this->_chatroom.get()->resign(this->_user);
				LOG_DEBUG("resigning current user");

				Object::release(this->_chatroom.get());
				this->_chatroom = NULL;
				LOG_DEBUG("releasing references to chatroom");

				this->_state = kInterfaceStateLobby;
				LOG_DEBUG("changing states to lobby");
				LOG_FLUSH;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) { // help
				this->_returnfromhelpstate = this->_state;
				this->_state = kInterfaceStateHelp;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_DRAFT)) { // draft
				this->_state = kInterfaceStateDraft;
				this->_updateconversation = true;
			} else {
				LOG_DEBUG("unknown: '%s'", userInput.cString());
			}

			userInput.reset();
		}
		break;
	case kInterfaceStateDraft:
		if (userInput.isready()) {
			// send buf
			this->_chatroom.get()->sendBuffer(userInput);

			this->_state = kInterfaceStateChatroom;

			userInput.reset();
		}
		break;
	case kInterfaceStateHelp:
		this->_state = this->_returnfromhelpstate;
		userInput.reset();
		break;
	}

	return 0;
}

int Interface::windowLoop() {
	BFThreadAsyncID tid = BFThreadAsync(Interface::displayWindowUpdateThread, (void *) this);
    InputBuffer userInput;
	this->_prevstate = kInterfaceStateUnknown;
	this->_state = kInterfaceStateLobby;
	while (this->_state.get() != kInterfaceStateQuit) {
		// draw ui based on current state
		this->draw();
		
		this->windowUpdateInputWindowText(userInput);

        int ch = wgetch(this->_inputWin); // Get user input
		userInput.addChar(ch);

		// `processinput` changes the current state of the interface
		this->_prevstate = this->_state;

		// act on current input state
		this->processinput(userInput);
    }

	BFThreadAsyncCancel(tid);
	BFThreadAsyncWait(tid);
	BFThreadAsyncDestroy(tid);

	return 0;
}

User * Interface::getuser() {
	while (!this->_user.get()) {
		// current this->_user is stil null
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
	return this->_user.get();
}

int Interface::gatherUserData() {
	// set up user
	char username[USER_NAME_SIZE];
	printf("username: ");
	fgets(username, sizeof(username), stdin);

	if (username[strlen(username) - 1] == '\n') {
		username[strlen(username)- 1] = '\0';
	}

	this->_user = User::create(username);
	BFRelease(this->_user.get());

	return 0;
}

int Interface::run() {
	int error = this->gatherUserData();

	this->windowStart();

	if (!error)
		error = this->windowLoop();

	this->windowStop();

	return error;
}

