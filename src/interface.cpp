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

using namespace BF;

const size_t linelen = DATA_BUFFER_SIZE + USER_NAME_SIZE + (2 << 4);
const int stateNormal = 0;
const int stateEdit = 1;
const int stateHelp = 2;
const int stateLobby = 3;
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
	BFLockCreate(&this->_winlock);
}

Interface::~Interface() {
	BFLockDestroy(&this->_winlock);
}

Interface * Interface::create(char mode) {
	return new Interface;
}

int InterfaceCraftChatLineFromMessage(const Message * msg, char * line) {
	if (!msg || !line) return 30;

	snprintf(line, linelen, "%s> %s", msg->username(), msg->data());
	return 0;
}

void Interface::displayWindowUpdateThread(void * in) {
	int error = 0;
	Interface * ui = (Interface *) ui;
	const BFThreadAsyncID tid = BFThreadAsyncGetID();

	while (!BFThreadAsyncIsCanceled(tid)) {
		ui->_chatroom->updateConversation.lock();
		if (ui->_chatroom->updateConversation.unsafeget()) {
			ui->_chatroom->conversation.lock();
			BFLockLock(&ui->_winlock);

			werase(ui->_displayWin);
			box(ui->_displayWin, 0, 0);

			// write messages
			for (int i = 0; i < ui->_chatroom->conversation.unsafeget().count(); i++) {
				Message * m = ui->_chatroom->conversation.unsafeget().objectAtIndex(i);

				if (m) {
					char line[linelen];
					InterfaceCraftChatLineFromMessage(m, line);
					mvwprintw(ui->_displayWin, i+1, 1, line);
				}
			}

			wrefresh(ui->_displayWin);
			
			ui->_chatroom->updateConversation.unsafeset(false);

			BFLockUnlock(&ui->_winlock);
			ui->_chatroom->conversation.unlock();
		}
		ui->_chatroom->updateConversation.unlock();
	}
}

int Interface::windowCreateModeLobby() {
	BFLockLock(&this->_winlock);

	if (this->_inputWin)
		delwin(this->_inputWin);
	if (this->_displayWin)
		delwin(this->_displayWin);
	
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

	if (this->_inputWin)
		delwin(this->_inputWin);
	if (this->_displayWin)
		delwin(this->_displayWin);
	
	// Create two windows
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);
	this->_displayWin = newwin(LINES - 1, COLS, 0, 0);

	box(this->_displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_inputWin); // Refresh the input window
	wrefresh(this->_displayWin); // Refresh the display window

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window

	this->_chatroom->updateConversation = true;

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateModeHelp() {
	BFLockLock(&this->_winlock);
	this->_helpWin = newwin(LINES - 10, COLS - 10, 5, 5);
	box(this->_helpWin, 0, 0); // Draw a box around the sub-window

	// dialog
	mvwprintw(this->_helpWin, 1, 3, " 'edit' : Draft message. To send hit enter key.");
	mvwprintw(this->_helpWin, 2, 3, " 'quit' : Quits application.");
	mvwprintw(this->_helpWin, LINES - 12, 3, "Press any key to close...");

	refresh(); // Refresh the main window to show the boxes
	wrefresh(this->_inputWin); // Refresh the input window
	wrefresh(this->_displayWin); // Refresh the display window
	wrefresh(this->_helpWin); // Refresh the help window

	keypad(this->_inputWin, true); // Enable special keys in input window
	nodelay(this->_inputWin, false); // Set blocking input for input window

	BFLockUnlock(&this->_winlock);
	return 0;
}

int Interface::windowCreateModeEdit() {
	BFLockLock(&this->_winlock);

	if (this->_helpWin)
		delwin(this->_helpWin);
	if (this->_inputWin)
		delwin(this->_inputWin);
	if (this->_displayWin)
		delwin(this->_displayWin);
	
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

int Interface::windowUpdateInputWindowText(InputBuffer & userInput, const int state) {
	BFLockLock(&this->_winlock);
	switch (state) {
	case stateNormal:
	case stateLobby:
		werase(this->_inputWin);
		mvwprintw(this->_inputWin, 0, 0, userInput.cString());
		wmove(this->_inputWin, 0, userInput.cursorPosition());
		wrefresh(this->_inputWin);
		break;
	case stateEdit:
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
    endwin(); // End curses mode

	return 0;
}

int Interface::windowLoop() {
	this->windowCreateModeLobby();

	//BFThreadAsyncID tid = BFThreadAsync(Interface::displayWindowUpdateThread, (void *) this);
    InputBuffer userInput;
	int state = stateLobby;
    while (1) {
        int ch = wgetch(this->_inputWin); // Get user input
		userInput.addChar(ch);
		if (state == stateNormal) { // normal
			if (!userInput.isready()) { 
				this->windowUpdateInputWindowText(userInput, state);
			} else {
				if (!userInput.compareString("quit")) {
					delwin(this->_inputWin);
					delwin(this->_displayWin);
					break; // exit loop
				} else if (!userInput.compareString("help")) {
					state = stateHelp;
					this->windowCreateModeHelp();
				} else if (!userInput.compareString("edit")) {
					state = stateEdit;

					// change to edit mode
					this->windowCreateModeEdit();
					this->_chatroom->updateConversation = true;
				} else {
					LOG_DEBUG("unknown: '%s'", userInput.cString());
				}

				userInput.reset();
			}
		} else if (state == stateEdit) { // edit
			if (!userInput.isready()) {
				this->windowUpdateInputWindowText(userInput, state);
			} else {
				// send buf
				this->_chatroom->sendBuffer(&userInput);

				state = stateNormal;

				this->windowCreateModeCommand();

				userInput.reset();
			}
		} else if (state == stateHelp) { // modal window
			this->windowCreateModeCommand();
			state = stateNormal;
			userInput.reset();
		} else if (state == stateLobby) { // lobby
			if (!userInput.isready()) {
				this->windowUpdateInputWindowText(userInput, state);
			} else {
				if (!userInput.compareString("quit")) {
					delwin(this->_inputWin);
					delwin(this->_displayWin);
					break; // exit loop
				}
				userInput.reset();
			}
		}
    }

	/*
	BFThreadAsyncCancel(tid);
	BFThreadAsyncWait(tid);
	BFThreadAsyncDestroy(tid);
	*/	
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

	/*
	if (!error) {
		if (ChatSocketGetMode() == SOCKET_MODE_SERVER) {
			error = InterfaceLobbyRunServer();
		} else {
			error = InterfaceLobbyRunClient();
		}
	}
	*/

	this->windowStart();

	if (!error)
		error = this->windowLoop();

	this->windowStop();

	return error;
}

