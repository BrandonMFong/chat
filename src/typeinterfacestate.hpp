/**
 * author: brando
 * date: 3/3/24
 */

#ifndef INTERFACE_STATE_HPP
#define INTERFACE_STATE_HPP

typedef enum {
	kInterfaceStateUnknown = 0,
	kInterfaceStateLobby = 1,
	kInterfaceStateDraft = 2,
	kInterfaceStateMessageViewer = 3,
	kInterfaceStateHelp = 4,
} InterfaceState;

#endif // INTERFACE_STATE_HPP

