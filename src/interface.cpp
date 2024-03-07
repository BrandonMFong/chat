/**
 * author: brando
 * date: 1/25/24
 */

#include "interface.hpp"
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

	BFLockCreate(&this->_winlock);
}

Interface::~Interface() {
	BFLockDestroy(&this->_winlock);
}

Interface * Interface::create(char mode) {
	return new Interface;
}

void Interface::chatroomListHasChanged() {
	this->_updatechatroomlist = true;
}

void Interface::converstaionHasChanged() {
	this->_updateconversation = true;
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
					LOG_DEBUG("updating chatroom list");

					BFLockLock(&interface->_winlock);
					werase(interface->_displayWin);
					box(interface->_displayWin, 0, 0);

					// print title
					int row = 1;
					mvwprintw(interface->_displayWin, row++, 1, "Chatrooms:");

					// get list
					int size = 0;
					int error = 0;
					PayloadChatInfo ** list = interface->_chatroom->getChatroomList(&size, &error);
					if (!list || error) {
						LOG_DEBUG("could not get list of chatrooms: %d", error);
					} else {
						LOG_DEBUG("found %d rooms", size);

						// show available rooms
						for (int i = 0; i < size; i++) { 
							char line[512];
							snprintf(line, 512, "(%d) \"%s\"", i, list[i]->chatroomname);
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
		delwin(this->_helpWin);

int Interface::windowCreateModeLobby() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;

	// Create two windows
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);
	this->_displayWin = newwin(LINES - 1, COLS, 0, 0);

	box(this->_displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_inputWin); // Refresh the input window
	wrefresh(this->_displayWin); // Refresh the display window

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateModeCommand() {
	// change to normal mode
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;
	
	// Create two windows
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);
	this->_displayWin = newwin(LINES - 1, COLS, 0, 0);

	box(this->_displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_inputWin); // Refresh the input window
	wrefresh(this->_displayWin); // Refresh the display window

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
	mvwprintw(this->_helpWin, 1, 3, " 'edit' : Draft message. To send hit enter key.");
	mvwprintw(this->_helpWin, 2, 3, " 'quit' : Quits application.");
	mvwprintw(this->_helpWin, LINES - 12, 3, "Press any key to close...");

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_helpWin); // Refresh the help window

	BFLockUnlock(&this->_winlock);
	return 0;
}

int Interface::windowCreateModeEdit() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;
	
	// Create two windows
	this->_inputWin = newwin(3, COLS, LINES - 3, 0);
	this->_displayWin = newwin(LINES - 3, COLS, 0, 0);

	box(this->_inputWin, 0, 0); // Add a box around the input window
	box(this->_displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_inputWin); // Refresh the input window
	wrefresh(this->_displayWin); // Refresh the display window

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window

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
		LOG_DEBUG("curr: %d, prev: %d",
			this->_state.get(),
			this->_prevstate.get());

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

int Interface::processinput(InputBuffer & userInput) {
	this->_prevstate = this->_state;
	switch (this->_state.get()) {
	case kInterfaceStateLobby:
		if (userInput.isready()) {
			Command cmd(userInput);
			if (!cmd.op().compareString("quit")) {
				this->_state = kInterfaceStateQuit;
			} else if (!cmd.op().compareString("create")) {
				// set up chat room name
				//
				// right now we are automatically creating a chatroom. The
				// user should be able to customize the room name
				char chatroomname[CHAT_ROOM_NAME_SIZE];
				snprintf(chatroomname, CHAT_ROOM_NAME_SIZE, "chatroom%d",
						Chatroom::getChatroomsCount());

				LOG_DEBUG("creating chatroom %s", chatroomname);
				this->_chatroom = ChatroomServer::create(chatroomname);
				BFRelease(this->_chatroom);
				LOG_DEBUG("creating chatroom was %sa success",
						this->_chatroom == NULL ? "not " : "");
			} else if (!cmd.op().compareString("join")) {

			}
			userInput.reset();
		}
		break;
	case kInterfaceStateChatroom:
		if (userInput.isready()) { 
			Command cmd(userInput);
			if (!cmd.op().compareString("help")) {
				this->_state = kInterfaceStateHelp;
			} else if (!cmd.op().compareString("edit")) {
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
		this->_state = this->_prevstate;
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
	while (this->_state != kInterfaceStateQuit) {
		this->windowUpdateInputWindowText(userInput);

		// draw ui based on current state
		this->draw();

        int ch = wgetch(this->_inputWin); // Get user input
		userInput.addChar(ch);

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

/*
int InterfaceLobbyRunClient() {
	LOG_DEBUG("waiting for this->_chatrooms");
	
	// ask server for list of chats
	AgentClient::getmain()->requestthis->_chatroomListUpdate(this->_user.get());

	// wait until one is available
	while (this->_chatroom::getthis->_chatroomsCount() == 0) { }

	int size = 0;
	int error = 0;
	PayloadChatInfo ** list = this->_chatroom::getthis->_chatroomList(&size, &error);
	if (!list || error)
		return error == 0 ? 1 : error;

	// choosing current this->_chatroom
	//
	// for now we are just choosing the first one
	AgentClient::getmain()->enrollInthis->_chatroom(list[0]);

	// save as current this->_chatroom
	this->_chatroom = this->_chatroom::getthis->_chatroom(list[0]->this->_chatroomuuid);
	if (!this->_chatroom)
		return 2;

	LOG_DEBUG("using this->_chatroom: %s", list[0]->this->_chatroomuuid);

	// free memory
	for (int i = 0; i < size; i++) { BFFree(list[i]); }
	BFFree(list);
	
	LOG_FLUSH;
	return 0;
}

int InterfaceLobbyRunServer() {
	// set up chat room name
	char this->_chatroomname[CHAT_ROOM_NAME_SIZE];

	printf("chat room name: ");
	fgets(this->_chatroomname, sizeof(this->_chatroomname), stdin);

	if (this->_chatroomname[strlen(this->_chatroomname) - 1] == '\n') {
		this->_chatroomname[strlen(this->_chatroomname) - 1] = '\0';
	}
	
	this->_chatroom = this->_chatroomServer::create(this->_chatroomname);
	BFRelease(this->_chatroom);
	
	LOG_FLUSH;
	
	return 0;
}
*/

int Interface::run() {
	int error = this->gatherUserData();

	this->windowStart();

	if (!error)
		error = this->windowLoop();

	this->windowStop();

	return error;
}

