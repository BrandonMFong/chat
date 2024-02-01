/**
 * author: brando
 * date: 1/25/24
 */

#include <interface.hpp>
#include <bflibcpp/bflibcpp.hpp>
#include <unistd.h>
#include <ncurses.h>

using namespace BF;

int InterfaceOutStreamAddMessage(ChatConfig * config, Packet * pkt) {
	if (!config || !pkt) return -2;

	Packet * p = PACKET_ALLOC;
	if (!p) return -2;

	memcpy(p, pkt, sizeof(Packet));

	config->out.lock();
	int error = config->out.get().push(p);
	config->out.unlock();
	return error;
}

void InterfaceInStreamThread(void * in) {
	ChatConfig * config = (ChatConfig *) in;

	while (1) {
		config->in.lock();
		// if queue is not empty, send the next message
		if (!config->in.get().empty()) {
			// get first message
			Packet * p = config->in.get().front();

			printf("> %s", p->payload.message.buf);
			fflush(stdout);

			// pop queue
			config->in.get().pop();

			PACKET_FREE(p);
		}
		config->in.unlock();
	}
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

int InterfaceRun(ChatConfig * config) {
	/*
	int i = 0;
	int error = 0;

	// this thread will monitor incoming messages from the in q
	BFThreadAsyncID tid = BFThreadAsync(InterfaceInStreamThread, (void *) config);

	while (!error) {
		Packet p;

		error = InterfaceReadInput(&p);

		if (!error)
			error = InterfaceOutStreamAddMessage(config, &p);

		i++;
	}
*/

    initscr(); // Initialize the library
    cbreak();  // Line buffering disabled, pass on everything to me

    // Create two windows
    WINDOW *inputWin = newwin(3, COLS, LINES - 3, 0);
    WINDOW *displayWin = newwin(LINES - 3, COLS, 0, 0);

    box(inputWin, 0, 0); // Add a box around the input window
    box(displayWin, 0, 0); // Add a box around the display window

    refresh(); // Refresh the main window to show the boxes
    wrefresh(inputWin); // Refresh the input window
    wrefresh(displayWin); // Refresh the display window

    keypad(inputWin, true); // Enable special keys in input window
    nodelay(inputWin, true); // Set non-blocking input for input window

	wmove(inputWin, 1, 1);
    std::string userInput;
    int ch;

    while (true) {
        ch = wgetch(inputWin); // Get user input

        if (ch != ERR) {
            // If a key is pressed, add it to the userInput string
            userInput.push_back(ch);
        }

        // Display user input in the display window
        werase(displayWin);
        mvwprintw(displayWin, 1, 1, userInput.c_str());
        wrefresh(displayWin);

        // Exit the loop if 'q' key is pressed
        if (ch == 'q') {
            break;
        }
    }

    endwin(); // End curses mode

 

	return 0;
}

