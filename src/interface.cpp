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
	BFRelease(this->_chatroom);
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

	snprintf(line, linelen, "%s> %s", msg->username(), msg->data());
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
				interface->_updatechatroomlist.lock();
				if (interface->_updatechatroomlist.unsafeget()) {

					BFLockLock(&interface->_winlock);
					werase(interface->_displayWin);
					box(interface->_displayWin, 0, 0);

					// print title
					int row = 1;
					mvwprintw(interface->_displayWin, row++, 1, "Chatrooms:");

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
							mvwprintw(interface->_displayWin, row++, 1, line);

							BFFree(list[i]);
						}
						BFFree(list);
					}

					wrefresh(interface->_displayWin);
					interface->_updatechatroomlist.unsafeset(false);
					BFLockUnlock(&interface->_winlock);
				}
				interface->_updatechatroomlist.unlock();
				break;
			}

			// allow the display window to always update its conversation
			// even when we are typing
			case kInterfaceStateChatroom:
			case kInterfaceStateDraft: 
			{
				interface->_updateconversation.lock();
				if (interface->_updateconversation.unsafeget()) {
					interface->_chatroom->conversation.lock();
					BFLockLock(&interface->_winlock);

					werase(interface->_displayWin);
					box(interface->_displayWin, 0, 0);

					// write messages
					for (int i = 0; i < interface->_chatroom->conversation.unsafeget().count(); i++) {
						Message * m = interface->_chatroom->conversation.unsafeget().objectAtIndex(i);

						if (m) {
							char line[linelen];
							InterfaceCraftChatLineFromMessage(m, line);
							mvwprintw(interface->_displayWin, i+1, 1, line);
						}
					}

					wrefresh(interface->_displayWin);
					
					interface->_updateconversation.unsafeset(false);

					BFLockUnlock(&interface->_winlock);
					interface->_chatroom->conversation.unlock();
				}
				interface->_updateconversation.unlock();
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
	snprintf(title, COLS, "%s", this->_chatroom->name());
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
	snprintf(title, COLS, "%s - draft", this->_chatroom->name());
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
			if (!cmd.op().compareString(INTERFACE_COMMAND_QUIT)) {
				this->_state = kInterfaceStateQuit;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) {
				this->_returnfromhelpstate = this->_state;
				this->_state = kInterfaceStateHelp;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_CREATE)) {
				// set up chat room name
				//
				// right now we are automatically creating a chatroom. The
				// user should be able to customize the room name
				char chatroomname[CHAT_ROOM_NAME_SIZE];
				snprintf(chatroomname, CHAT_ROOM_NAME_SIZE, "chatroom%d",
						Chatroom::getChatroomsCount());

				Chatroom * cr = ChatroomServer::create(chatroomname);
				BFRelease(cr);
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_JOIN)) {
				int index = String::toi(cmd[1]) - 1;
				if ((index >= 0) && (index < Chatroom::getChatroomsCount())) {
					this->_chatroom = _InterfaceGetChatroomAtIndex(index);
					if (this->_chatroom) {
						BFRetain(this->_chatroom);

						// enrolls current user on this machine to the chatroom
						this->_chatroom->enroll(this->_user.get());

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
			if (!cmd.op().compareString(INTERFACE_COMMAND_LEAVE)) {
				BFRelease(this->_chatroom);
				this->_chatroom = NULL;
				this->_state = kInterfaceStateLobby;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) {
				this->_returnfromhelpstate = this->_state;
				this->_state = kInterfaceStateHelp;
			} else if (!cmd.op().compareString(INTERFACE_COMMAND_DRAFT)) {
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
			this->_chatroom->sendBuffer(&userInput);

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

