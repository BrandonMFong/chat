/**
 * author: brando
 * date: 3/3/24
 */

#ifndef INTERFACE_STATE_HPP
#define INTERFACE_STATE_HPP

typedef enum {
	kInterfaceStateQuit = -1,
	kInterfaceStateUnknown = 0,
	kInterfaceStateLobby = 1,
	kInterfaceStateDraft = 2,
	kInterfaceStateChatroom = 3,
	kInterfaceStateHelp = 4,
	kInterfaceStateCreateChatroom = 5,
} InterfaceState;

#endif // INTERFACE_STATE_HPP

