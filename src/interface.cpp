/**
 * author: brando
 * date: 1/25/24
 */

#include "interface.hpp"
#include "user.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include "log.hpp"
#include "inputbuffer.hpp"
#include "office.hpp"
#include "chatroom.hpp"
#include "chatdirectory.hpp"

using namespace BF;

const size_t linelen = MESSAGE_BUFFER_SIZE + USER_NAME_SIZE;
const int stateNormal = 0;
const int stateEdit = 1;
const int stateHelp = 2;

BFLock winlock = 0;
WINDOW * inputWin = NULL;
WINDOW * displayWin = NULL;
WINDOW * helpWin = NULL;

Chatroom * chatroom;

void InterfaceMessageFree(Message * m) {
	MESSAGE_FREE(m);
}

int InterfaceCraftChatLineFromMessage(const Message * msg, char * line) {
	if (!msg || !line) return 30;

	snprintf(line, linelen, "%s> %s", msg->username, msg->buf);
	return 0;
}

void InterfaceDisplayWindowUpdateThread(void * in) {
	int error = 0;
	const BFThreadAsyncID tid = BFThreadAsyncGetID();

	LOG_DEBUG("> %s", __func__);
	while (BFThreadAsyncIDIsValid(tid) && !BFThreadAsyncIsCanceled(tid)) {
		chatroom->updateConversation.lock();
		if (chatroom->updateConversation.unsafeget()) {
			chatroom->conversation.lock();
			BFLockLock(&winlock);

			werase(displayWin);
			box(displayWin, 0, 0);

			LOG_DEBUG("updating display window");
			LOG_DEBUG("conversation message count: %d", chatroom->conversation.unsafeget().count());
			// write messages
			for (int i = 0; i < chatroom->conversation.unsafeget().count(); i++) {
				Message * m = chatroom->conversation.unsafeget().objectAtIndex(i);

				if (m) {
					char line[linelen];
					InterfaceCraftChatLineFromMessage(m, line);
					LOG_DEBUG("adding line to display: '%s'", line);
					mvwprintw(displayWin, i+1, 1, line);
				}
			}
			LOG_DEBUG("updating display window done");

			wrefresh(displayWin);
			
			LOG_DEBUG("%s:%d updating conversation", __func__, __LINE__);
			chatroom->updateConversation.unsafeset(false);

			BFLockUnlock(&winlock);
			chatroom->conversation.unlock();
		}
		chatroom->updateConversation.unlock();
	}
	LOG_DEBUG("< %s", __func__);
}

int InterfaceWindowCreateModeCommand() {
	// change to normal mode
	BFLockLock(&winlock);

	if (inputWin)
		delwin(inputWin);
	if (displayWin)
		delwin(displayWin);
	
	// Create two windows
	inputWin = newwin(1, COLS, LINES - 1, 0);
	displayWin = newwin(LINES - 1, COLS, 0, 0);

	box(displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(inputWin); // Refresh the input window
	wrefresh(displayWin); // Refresh the display window

	keypad(inputWin, true); // Enable special keys in input window
	nodelay(inputWin, false); // Set blocking input for input window

	LOG_DEBUG("%s:%d updating conversation", __func__, __LINE__);
	chatroom->updateConversation = true;

	BFLockUnlock(&winlock);

	return 0;
}

int InterfaceWindowCreateModeHelp() {
	BFLockLock(&winlock);
	helpWin = newwin(LINES - 10, COLS - 10, 5, 5);
	box(helpWin, 0, 0); // Draw a box around the sub-window

	// dialog
	mvwprintw(helpWin, 1, 3, " 'edit' : Draft message. To send hit enter key.");
	mvwprintw(helpWin, 2, 3, " 'quit' : Quits application.");
	mvwprintw(helpWin, LINES - 12, 3, "Press any key to close...");

	refresh(); // Refresh the main window to show the boxes
	wrefresh(inputWin); // Refresh the input window
	wrefresh(displayWin); // Refresh the display window
	wrefresh(helpWin); // Refresh the help window

	keypad(inputWin, true); // Enable special keys in input window
	nodelay(inputWin, false); // Set blocking input for input window

	BFLockUnlock(&winlock);
	return 0;
}

int InterfaceWindowCreateModeEdit() {
	BFLockLock(&winlock);

	if (helpWin)
		delwin(helpWin);
	if (inputWin)
		delwin(inputWin);
	if (displayWin)
		delwin(displayWin);
	
	// Create two windows
	inputWin = newwin(3, COLS, LINES - 3, 0);
	displayWin = newwin(LINES - 3, COLS, 0, 0);

	box(inputWin, 0, 0); // Add a box around the input window
	box(displayWin, 0, 0); // Add a box around the display window

	refresh(); // Refresh the main window to show the boxes
	wrefresh(inputWin); // Refresh the input window
	wrefresh(displayWin); // Refresh the display window

	keypad(inputWin, true); // Enable special keys in input window
	nodelay(inputWin, false); // Set blocking input for input window

	BFLockUnlock(&winlock);

	return 0;
}

int InterfaceWindowUpdateInputWindowText(InputBuffer & userInput, const int state) {
	BFLockLock(&winlock);
	if (state == stateNormal) {
		werase(inputWin);
		mvwprintw(inputWin, 0, 0, userInput.cString());
		wmove(inputWin, 0, userInput.cursorPosition());
		wrefresh(inputWin);
	} else if (state == stateEdit) {
		werase(inputWin);
		box(inputWin, 0, 0); // Add a box around the display window
		mvwprintw(inputWin, 1, 1, userInput.cString());
		wmove(inputWin, 1, userInput.cursorPosition() + 1);
		wrefresh(inputWin);
	}
	BFLockUnlock(&winlock);

	return 0;
}

int InterfaceWindowStart() {
	BFLockCreate(&winlock);

	initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me
    noecho();  // Don't echo user input

	return 0;
}

int InterfaceWindowEnd() {
    endwin(); // End curses mode

	BFLockDestroy(&winlock);
	return 0;
}

int InterfaceWindowLoop() {

	InterfaceWindowCreateModeCommand();

	BFThreadAsyncID tid = BFThreadAsync(InterfaceDisplayWindowUpdateThread, 0);
    InputBuffer userInput;
	int state = stateNormal; // 0 = normal, 1 = edit
    while (1) {
        int ch = wgetch(inputWin); // Get user input
		userInput.addChar(ch);
		if (state == stateNormal) { // normal
			if (!userInput.isready()) { 
				InterfaceWindowUpdateInputWindowText(userInput, state);
			} else {
				if (!userInput.compareString("quit")) {
					delwin(inputWin);
					delwin(displayWin);
					break; // exit loop
				} else if (!userInput.compareString("help")) {
					state = stateHelp;
					InterfaceWindowCreateModeHelp();
				} else if (!userInput.compareString("edit")) {
					LOG_DEBUG("state changed to edit");
					state = stateEdit;

					// change to edit mode
					InterfaceWindowCreateModeEdit();
					LOG_DEBUG("%s:%d updating conversation", __func__, __LINE__);
					chatroom->updateConversation = true;
				} else {
					LOG_DEBUG("unknown: '%s'", userInput.cString());
				}

				userInput.reset();
			}
		} else if (state == stateEdit) { // edit
			if (!userInput.isready()) {
				InterfaceWindowUpdateInputWindowText(userInput, state);
			} else {
				// send buf
				chatroom->sendBuffer(&userInput);

				state = stateNormal;

				InterfaceWindowCreateModeCommand();

				userInput.reset();
			}
		} else if (state == stateHelp) { // modal window
			InterfaceWindowCreateModeCommand();
			state = stateNormal;
			userInput.reset();
		}
    }

	BFThreadAsyncCancel(tid);
	BFThreadAsyncWait(tid);
	BFThreadAsyncDestroy(tid);
	
	return 0;
}

int InterfaceGatherUserData() {
	// set up user
	User * currentuser = User::current();
	char username[USER_NAME_SIZE];
	printf("username: ");
	fgets(username, sizeof(username), stdin);

	if (username[strlen(username) - 1] == '\n') {
		username[strlen(username)- 1] = '\0';
	}

	currentuser->setUsername(username);

	return 0;
}

int InterfaceLobbyRun() {
	LOG_DEBUG("> %s", __func__);
	// set up chat room name
	char chatroomname[CHAT_ROOM_NAME_SIZE];
	chatroom = new Chatroom("ea46019c-4c39-4838-b44d-6a990bbb4ae9");

	printf("chat room name: ");
	fgets(chatroomname, sizeof(chatroomname), stdin);

	if (chatroomname[strlen(chatroomname) - 1] == '\n') {
		chatroomname[strlen(chatroomname)- 1] = '\0';
	}
	
	chatroom->setName(chatroomname);
	ChatDirectory::shared()->addChatroom(chatroom);
	LOG_DEBUG("< %s", __func__);
	
	return 0;
}

int InterfaceRun() {
	LOG_DEBUG("> %s", __func__);

	int error = InterfaceGatherUserData();

	if (!error)
		error = InterfaceLobbyRun();

	InterfaceWindowStart();

	if (!error)
		error = InterfaceWindowLoop();

	InterfaceWindowEnd();

	Delete(chatroom);

	LOG_DEBUG("< %s", __func__);
	return error;
}

