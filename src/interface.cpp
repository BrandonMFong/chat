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

void InterfaceInStreamQueueCallback(const Packet & p) {
	// If Enter key is pressed, display user input in the display window
	werase(displayWin);
	box(displayWin, 0, 0);
	mvwprintw(displayWin, 1, 1, p.payload.message.buf);
	wrefresh(displayWin);
}

int InterfaceReadInput(Packet * p) {
	if (!p) return -30;
	printf("> ");
	fgets(
		p->payload.message.buf,
		sizeof(p->payload.message.buf),
		stdin
	);

	if (strlen(p->payload.message.buf)
	&& p->payload.message.buf[strlen(p->payload.message.buf) - 1] == '\n') {
		p->payload.message.buf[strlen(p->payload.message.buf) - 1] = '\0';
	}

	return 0;
}

int InterfaceWindowLoop(Socket * skt) {
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

    String userInput;

    while (1) {
		Packet p;
        int ch = wgetch(inputWin); // Get user input

        if (ch == '\n') {
			// load packet
			strncpy(p.payload.message.buf, userInput.cString(), sizeof(p.payload.message.buf));

			skt->sendPacket(&p);
			
            // Clear the input window and userInput
            werase(inputWin);
            box(inputWin, 0, 0);
            wrefresh(inputWin);
            userInput = "";
        } else if (ch != ERR) {
            // If a key is pressed (excluding Enter), add it to the userInput string
            userInput.addChar(ch);
        }

        // Display user input in the input window
        mvwprintw(inputWin, 1, 1, userInput.cString());
        wrefresh(inputWin);

        // Exit the loop if 'q' key is pressed
        if (ch == 'q') {
            break;
        }
    }

    endwin(); // End curses mode

	return 0;
}

int InterfaceRun(Socket * skt) {
	int error = 0;
	/*
	while (!error) {
		Packet p;
		error = InterfaceReadInput(&p);
		if (!error) {
			if (!strcmp(p.payload.message.buf, "q")) break;
			skt->sendPacket(&p);
		}
	}
	*/

	Packet p;
	InterfaceWindowLoop(skt);
	
	return 0;
}

