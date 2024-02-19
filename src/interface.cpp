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

using namespace BF;

const size_t linelen = MESSAGE_BUFFER_SIZE + USER_NAME_SIZE;

BFLock winlock = 0;
WINDOW * inputWin = NULL;
WINDOW * displayWin = NULL;

Atomic<List<Message *>> conversation;
Atomic<bool> updateConversation;

void InterfaceMessageFree(Message * m) {
	MESSAGE_FREE(m);
}

void InterfaceConversationAddMessage(const Message * msg) {
	Message * m = MESSAGE_ALLOC;
	memcpy(m, msg, sizeof(Message));
	conversation.get().add(m);
	updateConversation = true;
}

void InterfaceInStreamQueueCallback(const Packet & p) {
	InterfaceConversationAddMessage(&p.payload.message);
}

int InterfaceCraftChatLineFromMessage(const Message * msg, char * line) {
	if (!msg || !line) return 30;

	snprintf(line, linelen, "%s> %s", msg->username, msg->buf);
	return 0;
}

void InterfaceDisplayWindowUpdateThread(void * in) {
	int error = 0;
	const BFThreadAsyncID tid = BFThreadAsyncGetID();

	while (BFThreadAsyncIDIsValid(tid) && !BFThreadAsyncIsCanceled(tid)) {
		updateConversation.lock();
		if (updateConversation.get()) {
			BFLockLock(&winlock);
			conversation.lock();
			
			werase(displayWin);
			box(displayWin, 0, 0);

			// write messages
			for (int i = 0; i < conversation.get().count(); i++) {
				Message * m = conversation.get().objectAtIndex(i);

				if (m) {
					char line[linelen];
					InterfaceCraftChatLineFromMessage(m, line);
					mvwprintw(displayWin, i+1, 1, line);
				}
			}

			wrefresh(displayWin);
		
			updateConversation = false;	
			
			conversation.unlock();
			BFLockUnlock(&winlock);
		}
		updateConversation.unlock();
	}
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

	updateConversation = true;

	BFLockUnlock(&winlock);

	return 0;
}

int InterfaceWindowCreateModeEdit() {
	BFLockLock(&winlock);

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

int _InterfaceWindowLoop(Socket * skt) {
	BFLockCreate(&winlock);

	initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me
    noecho();  // Don't echo user input

	InterfaceWindowCreateModeCommand();

	// setup conversation thread
	conversation.get().setDeallocateCallback(InterfaceMessageFree);

	BFThreadAsyncID tid = BFThreadAsync(InterfaceDisplayWindowUpdateThread, 0);
    InputBuffer userInput;
	int state = 0; // 0 = normal, 1 = edit
    while (1) {
		Packet p;
        int ch = wgetch(inputWin); // Get user input
		userInput.addChar(ch);
		if (state == 0) { // normal
			if (!userInput.isready()) { 
				// Display user input in the input window
				BFLockLock(&winlock);
				werase(inputWin);
				mvwprintw(inputWin, 0, 0, userInput.cString());
				wmove(inputWin, 0, userInput.cursorPosition());
				wrefresh(inputWin);
				BFLockUnlock(&winlock);
			} else {
				if (!userInput.compareString(":quit")) {
					break; // exit loop
				} else if (!userInput.compareString(":edit")) {
					LOG_DEBUG("state changed to edit");
					state = 1;

					// change to edit mode
					InterfaceWindowCreateModeEdit();
					updateConversation = true;
				} else {
					LOG_DEBUG("unknown: '%s'", userInput.cString());
				}

				userInput.reset();
			}
		} else if (state == 1) { // edit
			if (!userInput.isready()) {
				// Display user input in the input window
				BFLockLock(&winlock);
				werase(inputWin);
				box(inputWin, 0, 0); // Add a box around the display window
				mvwprintw(inputWin, 1, 1, userInput.cString());
				wmove(inputWin, 1, userInput.cursorPosition() + 1);
				wrefresh(inputWin);
				BFLockUnlock(&winlock);
			} else {
				// load packet
				userInput.unload(&p);

				// Add message to our display
				InterfaceConversationAddMessage(&p.payload.message);

				// Send packet
				skt->sendPacket(&p);

				state = 0;

				InterfaceWindowCreateModeCommand();

				userInput.reset();
			}
		}
    }

	BFThreadAsyncCancel(tid);
	BFThreadAsyncWait(tid);
	BFThreadAsyncDestroy(tid);

	delwin(inputWin);
	delwin(displayWin);
    endwin(); // End curses mode

	BFLockDestroy(&winlock);

	return 0;
}
/*
int InterfaceWindowLoop(Socket * skt) {
	BFLockCreate(&winlock);

	initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me
    noecho();  // Don't echo user input

    // Create two windows
    inputWin = newwin(1, COLS, LINES - 1, 0);
    displayWin = newwin(LINES - 1, COLS, 0, 0);

    //box(inputWin, 0, 0); // Add a box around the input window
    box(displayWin, 0, 0); // Add a box around the display window

    refresh(); // Refresh the main window to show the boxes
    wrefresh(inputWin); // Refresh the input window
    wrefresh(displayWin); // Refresh the display window

    keypad(inputWin, true); // Enable special keys in input window
    nodelay(inputWin, false); // Set blocking input for input window

	// setup conversation thread
	conversation.get().setDeallocateCallback(InterfaceMessageFree);

	BFThreadAsyncID tid = BFThreadAsync(InterfaceDisplayWindowUpdateThread, 0);

    InputBuffer userInput;

    while (1) {
		Packet p;
        int ch = wgetch(inputWin); // Get user input
		userInput.addChar(ch);

		if (ch == 'q') {
			break;
		} else if (userInput.isready()) {
			// load packet
			userInput.unload(&p);

			// Add message to our display
			InterfaceConversationAddMessage(&p.payload.message);

			// Send packet
			skt->sendPacket(&p);
			
            // Clear the input window and userInput
			BFLockLock(&winlock);
            werase(inputWin);
            wrefresh(inputWin);
            userInput.clear();
			BFLockUnlock(&winlock);
		}

        // Display user input in the input window
		BFLockLock(&winlock);
		werase(inputWin);
        mvwprintw(inputWin, 0, 0, userInput.cString());
		wmove(inputWin, 0, userInput.cursorPosition());
        wrefresh(inputWin);
		BFLockUnlock(&winlock);
    }

	BFThreadAsyncCancel(tid);
	BFThreadAsyncWait(tid);
	BFThreadAsyncDestroy(tid);

	delwin(inputWin);
	delwin(displayWin);
    endwin(); // End curses mode

	BFLockDestroy(&winlock);

	return 0;
}
*/
int InterfaceGatherUserData() {
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

int InterfaceRun(Socket * skt) {
	int error = InterfaceGatherUserData();

	return _InterfaceWindowLoop(skt);
}

