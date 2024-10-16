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
#include "permissions.hpp"

using namespace BF;

const size_t kInterfaceConversationLineLength = PAYLOAD_MESSAGE_LIMIT_MESSAGE + USER_NAME_SIZE + (2 << 4);
const size_t kInterfaceMessageLengthLimit = PAYLOAD_MESSAGE_LIMIT_MESSAGE;
Interface * interface = NULL;

Interface * Interface::current() {
	return interface;
}

Interface::Interface() {
	interface = this;
	this->_inputWin = NULL;
	this->_displayWin = NULL;
	this->_helpWin = NULL;
	this->_headerWin = NULL;
	this->_chatroom = NULL;
	this->_state = kInterfaceStateUnknown;
	this->_prevstate = kInterfaceStateUnknown;
	this->_updatelobby = false;
	this->_updateconversation = false;
	this->_user = NULL;

	BFLockCreate(&this->_winlock);
	BFLockCreate(&this->_uistoplight);
}

Interface::~Interface() {
	BFLockDestroy(&this->_winlock);
	BFLockDestroy(&this->_uistoplight);
	Object::release(this->_chatroom.get());
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

void Interface::userListHasChanged() {
	this->lobbyHasChanged();
}

void Interface::chatroomListHasChanged() {
	this->lobbyHasChanged();
}

void Interface::lobbyHasChanged() {
	this->_updatelobby = true;
	BFLockRelease(&this->_uistoplight);
}

void Interface::converstaionHasChanged() {
	this->_updateconversation = true;
	BFLockRelease(&this->_uistoplight);
}

InterfaceState Interface::currstate() {
	return this->_state.get();
}

InterfaceState Interface::prevstate() {
	return this->_prevstate.get();
}

void Interface::setErrorMessage(String errmsg) {
	this->_errorMessage = errmsg;
}

int InterfaceCraftChatLineFromMessage(const Message * msg, char * line) {
	if (!msg || !line) return 30;

	switch (msg->type()) {
	case kPayloadMessageTypeData:
		snprintf(line, kInterfaceConversationLineLength, "%s> %s", msg->username(), msg->data());
		return 0;
	case kPayloadMessageTypeUserJoined:
	case kPayloadMessageTypeUserLeft:
		char username[USER_NAME_SIZE];
		uuid_t u0, u1;
		Interface::current()->getuser()->getuuid(u0);
		msg->getuuiduser(u1);
		if (!uuid_compare(u0, u1)) {
			strncpy(username, "You", USER_NAME_SIZE);
		} else {
			strncpy(username, msg->username(), USER_NAME_SIZE);
		}

		if (msg->type() == kPayloadMessageTypeUserJoined) {
			snprintf(line, kInterfaceConversationLineLength, "<<%s joined the chat>>", username);
		} else if (msg->type() == kPayloadMessageTypeUserLeft) {
			snprintf(line, kInterfaceConversationLineLength, "<<%s left the chat>>", username);
		}

		return 0;
	default:
		return 31;
	}
}

int _InterfaceFixTextInBoxedWindow(WINDOW * win, const char * text, int startingline) {
	int w, h;
	getmaxyx(win, h, w);
	const int boxwidth = w - 2;
	const int lines = (strlen(text) / boxwidth) + 1;

	char * cstr = BFStringCopyString(text);
	char * buf = cstr;
	char tmp[boxwidth + 1];
	for (int i = 0; i < lines; i++) {
		int num = boxwidth;
		if (strlen(buf) < num) {
			num = strlen(buf);
		}
		strncpy(tmp, buf, num);
		tmp[num] = '\0';
		mvwprintw(win, i + startingline, 1, tmp);
		wmove(win, i + startingline, strlen(tmp) + 1);

		buf += boxwidth;
	}

	BFFree(cstr);

	return 0;
}

/**
 * row : gets modified 
 */
void _InterfaceDrawMessage(
	WINDOW * dispwin,
	int & row,
	int col,
	const Message * m
) {
	if (!m || !dispwin)
		return;

	char line[kInterfaceConversationLineLength];
	if (!InterfaceCraftChatLineFromMessage(m, line)) {
		int w, h;
		getmaxyx(dispwin, h, w);
		const int boxwidth = w - 2;
		const int lines = (strlen(line) / boxwidth);
		row -= lines;

		// only print if we have space
		if (row > 0) {
			_InterfaceFixTextInBoxedWindow(dispwin, line, row--);
		}
	}
}

// this will write conversation from the bottom to the top
int Interface::windowWriteConversation() {
	BFLockLock(&this->_winlock);
	this->_updateconversation.lock();
	if (this->_updateconversation.unsafeget() && this->_chatroom.get()) {
		this->_chatroom.lock();
		this->_chatroom.unsafeget()->conversation.lock();

		int w, h;
		getmaxyx(this->_displayWin, h, w);

		werase(this->_displayWin);

		// write messages
		int row = h - 2; // row to start the messages on (account for header and box)
		const int rowtostop = 0;
		List<Message *>::Node * n = this->_chatroom.unsafeget()->conversation.unsafeget().last();
		while (n && (row > rowtostop)) {
			Message * m = n->object();
			
			_InterfaceDrawMessage(this->_displayWin, row, 1, m);

			n = n->prev();
		}

		box(this->_displayWin, 0, 0);
		wrefresh(this->_displayWin);
		
		this->_updateconversation.unsafeset(false);

		this->_chatroom.unsafeget()->conversation.unlock();
		this->_chatroom.unlock();
	}
	this->_updateconversation.unlock();
	BFLockUnlock(&this->_winlock);
	return 0;
}

int Interface::windowWriteContentLobby() {
	BFLockLock(&this->_winlock);
	this->_updatelobby.lock();
	if (this->_updatelobby.unsafeget()) {
		this->drawDisplayWindowLobby();

		this->windowWriteChatroomList();
		this->windowWriteUserList();

		wrefresh(this->_displayWin);
		this->_updatelobby.unsafeset(false);
	}
	this->_updatelobby.unlock();
	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowWriteChatroomList() {
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
	return 0;
}

int _InterfaceCraftLobbyUserLine(PayloadUserInfo * userinfo, char * line, User * curruser) {
	if (!userinfo || !line || !curruser)
		return 1;

	uuid_t uuid;
	curruser->getuuid(uuid);
	if (!uuid_compare(uuid, userinfo->useruuid)) {
		snprintf(line, 512, "You (%s)", userinfo->username);
	} else {
		snprintf(line, 512, "%s", userinfo->username);
	}

	return 0;
}

int Interface::windowWriteUserList() {
	int w, h;
    getmaxyx(this->_displayWin, h, w);

	// print title
	int row = 1;
	mvwprintw(this->_displayWin, row++, (w/2) + 1, "Users:");

	// get list
	int size = 0;
	int error = 0;
	PayloadUserInfo ** list = User::getUserList(&size, &error);
	if (!list || error) {
		LOG_DEBUG("could not get list of users: %d", error);
	} else {
		for (int i = 0; i < size; i++) { 
			char line[512];
			_InterfaceCraftLobbyUserLine(list[i], line, this->_user.get());
			mvwprintw(this->_displayWin, row++, (w/2) + 1, line);

			BFFree(list[i]);
		}
		BFFree(list);
	}

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
				interface->windowWriteContentLobby();
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
			default:
				break;
		}

		// this should get released when something has changed
		//
		// see header
		BFLockWait(&interface->_uistoplight);
	}
}

#define DELETE_WINDOWS \
	if (this->_inputWin) {\
		delwin(this->_inputWin);this->_inputWin = NULL; }\
	if (this->_displayWin) {\
		delwin(this->_displayWin);this->_displayWin = NULL; }\
	if (this->_helpWin) {\
		delwin(this->_helpWin);this->_helpWin = NULL; }\
	if (this->_headerWin) {\
		delwin(this->_headerWin);this->_headerWin = NULL; }

int Interface::drawDisplayWindowLobby() {
	int w, h;
    getmaxyx(this->_displayWin, h, w);

	// clear
	werase(this->_displayWin);

	// draw a box around window
	box(this->_displayWin, 0, 0);

	// draw a line down the middle
	mvwvline(this->_displayWin, 1, w/2, ACS_VLINE, h - 2);

	return 0;
}

int Interface::windowCreateModeLobby() {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;

	// Create two windows
	this->_headerWin = newwin(1, COLS, 0, 0);
	this->_displayWin = newwin(LINES - 2, COLS, 1, 0);
	this->_inputWin = newwin(1, COLS, LINES - 1, 0);

	this->drawDisplayWindowLobby();

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
	
	this->_updatelobby = true;
	BFLockRelease(&this->_uistoplight);

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateStateChatroom() {
	// change to normal mode
	BFLockLock(&this->_winlock);

	erase();
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

	this->converstaionHasChanged();

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateStateDraft(int inputWinWidth, int inputWinHeight) {
	BFLockLock(&this->_winlock);

	DELETE_WINDOWS;
	
	// Create two windows
	this->_headerWin = newwin(1, COLS, 0, 0);
	this->_displayWin = newwin(LINES - inputWinHeight - 1, COLS, 1, 0);
	this->_inputWin = newwin(inputWinHeight, inputWinWidth, LINES - inputWinHeight, 0);

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
	
	this->converstaionHasChanged();

	BFLockUnlock(&this->_winlock);

	return 0;
}

int Interface::windowCreateStateDraft() {
	int inputWinWidth = COLS;
   	int inputWinHeight = 3;
	return this->windowCreateStateDraft(inputWinWidth, inputWinHeight);
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

int _InterfaceDrawUserInputDraft(
	BFLock * winlock,
	WINDOW * inputwin,
	InputBuffer & userInput
) {
	BFLockLock(winlock);
	werase(inputwin);
	box(inputwin, 0, 0);

	_InterfaceFixTextInBoxedWindow(inputwin, userInput, 1);

	wrefresh(inputwin);

	BFLockUnlock(winlock);

	return 0;
}

int Interface::windowUpdateInputWindowText(InputBuffer & userInput) {
	switch (this->_state.get()) {
	case kInterfaceStateChatroom:
	case kInterfaceStateLobby:
	{
		BFLockLock(&this->_winlock);
		werase(this->_inputWin);
		if (this->_errorMessage.length() == 0) {
			mvwprintw(this->_inputWin, 0, 0, userInput.cString());
		} else {
			mvwprintw(this->_inputWin, 0, 0, this->_errorMessage.cString());
			this->_errorMessage.clear();
		}
		wmove(this->_inputWin, 0, userInput.cursorPosition());
		wrefresh(this->_inputWin);
		BFLockUnlock(&this->_winlock);
		break;
	}
	case kInterfaceStateDraft:
	{
		int w, h;
		getmaxyx(this->_inputWin, h, w);
		const int boxwidth = w - 2;
		const int lines = (userInput.length() / boxwidth) + 1;

		// see if we need to expand the height for the input window
		if (lines > 1) { // change window to fit text
			const int off = 2;
			this->windowCreateStateDraft(w, lines + off);
		}

		_InterfaceDrawUserInputDraft(&this->_winlock, this->_inputWin, userInput);

		break;
	}
	default:
		break;
	}

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
			this->windowCreateStateChatroom();
			break;
		case kInterfaceStateDraft:
			this->windowCreateStateDraft();
			break;
		case kInterfaceStateHelp:
			this->windowCreateModeHelp();
			break;
		default:
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

int Interface::processinputStateLobby(InputBuffer & userInput) {
	if (userInput.isready()) {
		Command cmd(userInput);
		if (!cmd.op().compareString(INTERFACE_COMMAND_QUIT)) { // quit
			Office::quitApplication(this->_user.get());
			this->_state = kInterfaceStateQuit;
		} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) { // help
			this->_returnfromhelpstate = this->_state;
			this->_state = kInterfaceStateHelp;
		} else if (!cmd.op().compareString(INTERFACE_COMMAND_CREATE)) { // create
			if (!Permissions::CanCreateChatroom()) {
				String * errmsg = String::createWithFormat("not permitted: you are not allowd to create a chatroom");
				this->setErrorMessage(*errmsg);
				BFRelease(errmsg);
			} else {
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
			}
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
		} else {
			String * errmsg = String::createWithFormat("unknown command: %s", cmd.op().cString());
			this->setErrorMessage(*errmsg);
			BFRelease(errmsg);
		}
		userInput.reset();
	}

	return 0;
}

int Interface::processinputStateChatroom(InputBuffer & userInput) {
	if (userInput.isready()) { 
		Command cmd(userInput);
		if (!cmd.op().compareString(INTERFACE_COMMAND_LEAVE)) { // leave
			// tell chat room we are leaving
			this->_chatroom.get()->resign(this->_user);

			Object::release(this->_chatroom.get());
			this->_chatroom = NULL;

			this->_state = kInterfaceStateLobby;
		} else if (!cmd.op().compareString(INTERFACE_COMMAND_HELP)) { // help
			this->_returnfromhelpstate = this->_state;
			this->_state = kInterfaceStateHelp;
		} else if (!cmd.op().compareString(INTERFACE_COMMAND_DRAFT)) { // draft
			this->_state = kInterfaceStateDraft;
			this->converstaionHasChanged();
		} else {
			String * errmsg = String::createWithFormat("unknown command: %s", cmd.op().cString());
			this->setErrorMessage(*errmsg);
			BFRelease(errmsg);
		}

		userInput.reset();
	}
	return 0;
}

int Interface::processinputStateDraft(InputBuffer & userInput) {
	if (userInput.isready()) {
		// send buf
		this->_chatroom.get()->sendBuffer(userInput);

		this->_state = kInterfaceStateChatroom;

		userInput.reset();
	} else if (userInput.length() >= kInterfaceMessageLengthLimit) {
		userInput.remChar();
	}

	return 0;
}

int Interface::processinput(InputBuffer & userInput) {
	switch (this->_state.get()) {
	case kInterfaceStateLobby:
		this->processinputStateLobby(userInput);
		break;
	case kInterfaceStateChatroom:
		this->processinputStateChatroom(userInput);
		break;
	case kInterfaceStateDraft:
		this->processinputStateDraft(userInput);
		break;
	case kInterfaceStateHelp:
		this->_state = this->_returnfromhelpstate;
		userInput.reset();
		break;
	default:
		break;
	}

	return 0;
}

int Interface::readUserInput(InputBuffer & userInput) {
	WINDOW * win = NULL;
	switch (this->_state.get()) {
	case kInterfaceStateHelp:
		win = this->_helpWin;
		break;
	default:
		win = this->_inputWin;
		break;
	}
	
	// this gets blocked
	int ch = wgetch(win); // Get user input
	userInput.addChar(ch);

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

		this->readUserInput(userInput);

		// `processinput` changes the current state of the interface
		this->_prevstate = this->_state;

		// act on current input state
		this->processinput(userInput);
    }

	BFLockRelease(&this->_uistoplight);

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

int Interface::run() {
	int error = this->gatherUserData();

	this->windowStart();

	if (!error)
		error = this->windowLoop();

	this->windowStop();

	return error;
}

