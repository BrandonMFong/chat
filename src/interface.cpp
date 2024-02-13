/**
 * author: brando
 * date: 1/25/24
 */

#include <interface.hpp>
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>
#include <ncurses.h>

using namespace BF;

WINDOW * inputWin = NULL;
WINDOW * displayWin = NULL;

BFLock winlock = 0;

Atomic<List<Message *>> conversation;

void InterfaceMessageFree(Message * m) {
	MESSAGE_FREE(m);
}

void InterfaceConversationAddMessage(const Message * msg) {
	Message * m = MESSAGE_ALLOC;
	memcpy(m, msg, sizeof(Message));
	conversation.get().add(m);
}

void InterfaceInStreamQueueCallback(const Packet & p) {
	InterfaceConversationAddMessage(&p.payload.message);
}

void InterfaceDisplayWindowUpdateThread(void * in) {
	Atomic<bool> * dowork = (Atomic<bool> *) in;
	int error = 0;
	int messagecount = conversation.get().count();

	while (dowork->get()) {
		conversation.lock();
		if (conversation.get().count() != messagecount) {
			BFLockLock(&winlock);
			werase(displayWin);
			box(displayWin, 0, 0);

			// write messages
			for (int i = 0; i < conversation.get().count(); i++) {
				Message * m = conversation.get().objectAtIndex(i);
				if (m)
					mvwprintw(displayWin, i+1, 1, m->buf);
			}

			wrefresh(displayWin);
			
			messagecount = conversation.get().count(); // update count
			
			BFLockUnlock(&winlock);
		}
		conversation.unlock();
	}
}

int InterfaceWindowLoop(Socket * skt) {
	BFLockCreate(&winlock);

	initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me
    noecho();  // Don't echo user input

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

	// setup conversation thread
	conversation.get().setDeallocateCallback(InterfaceMessageFree);

	Atomic<bool> dowork(true);
	BFThreadAsyncID tid = BFThreadAsync(InterfaceDisplayWindowUpdateThread, &dowork);

    String userInput;

    while (1) {
		Packet p;
        int ch = wgetch(inputWin); // Get user input

        if (ch == '\n') {
			// load packet
			strncpy(p.payload.message.buf, userInput.cString(), sizeof(p.payload.message.buf));
			
			InterfaceConversationAddMessage(&p.payload.message);

			skt->sendPacket(&p);
			
            // Clear the input window and userInput
			BFLockLock(&winlock);
            werase(inputWin);
            box(inputWin, 0, 0);
            wrefresh(inputWin);
            userInput = "";
			BFLockUnlock(&winlock);
        } else if (ch != ERR) {
            // If a key is pressed (excluding Enter), add it to the userInput string
            userInput.addChar(ch);
        }

        // Display user input in the input window
		BFLockLock(&winlock);
        mvwprintw(inputWin, 1, 1, userInput.cString());
        wrefresh(inputWin);
		BFLockUnlock(&winlock);

        // Exit the loop if 'q' key is pressed
        if (ch == 'q') {
            break;
        }
    }

	dowork = false;
	while (BFThreadAsyncIDIsRunning(tid)) { }
	BFThreadAsyncCancel(tid);
	BFThreadAsyncIDDestroy(tid);

	delwin(inputWin);
	delwin(displayWin);
    endwin(); // End curses mode

	BFLockDestroy(&winlock);

	return 0;
}

int InterfaceRun(Socket * skt) {
	return InterfaceWindowLoop(skt);
}

