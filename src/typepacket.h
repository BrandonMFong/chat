/**
 * author: brando
 * date: 1/25/24
 */

#ifndef TYPE_PACKET_H
#define TYPE_PACKET_H

#include <uuid/uuid.h>

extern "C" {
#include <bflibc/bftime.h>
#include <bflibc/stringutils.h>
}

#define USER_NAME_SIZE 255
#define CHAT_ROOM_NAME_SIZE 255
#define DATA_BUFFER_SIZE 255

typedef enum {
	kPayloadTypeUnknown = 0,

	/**
	 * payload holds a message
	 */
	kPayloadTypeMessage = 1,

	/**
	 * empty payload. type signals receiver
	 * send information about their current user
	 */
	kPayloadTypeRequestUserInfo = 2,

	/**
	 * information about the user who sent 
	 * data
	 */
	kPayloadTypeUserInfo = 3,

	/**
	 * returns brief information of all chatrooms
	 * that are available to join
	 */
	kPayloadTypeRequestChatroomList = 4,

	/**
	 * contains chat room information
	 *
	 * should be enough information for the user to use
	 * to determine whether they want to join it or not
	 */
	kPayloadTypeChatInfo = 5,

	/**
	 * a notification that a user is joining a classroom
	 * 
	 * receiver should note the agent associated with
	 * packet
	 */
	kPayloadTypeChatroomEnrollment = 6,

	/**
	 * tells receiver that chatroom list changed on
	 * server.  All clients will go through the process
	 * of updating their own list of chatrooms
	 */
	kPayloadTypeNotifyChatroomListChanged = 7,

	/**
	 * gets sent when user left chatroom
	 */
	kPayloadTypeChatroomResignation = 8,

	/**
	 * when user `quit`s the app, a broadcast message
	 * will be sent to all users to notify them that
	 * user has left the server
	 *
	 * uses PayloadUserInfo payload
	 */
	kPayloadTypeNotifyQuitApp = 9,

	/**
	 * a form that bundles requests/responses 
	 * for enrollment
	 */
	kPayloadTypeChatroomEnrollmentForm = 10,
} PayloadType;

typedef enum {
	kPayloadMessageTypeData = 0,
	kPayloadMessageTypeUserJoined = 1,
	kPayloadMessageTypeUserLeft = 2,
} PayloadMessageType;

typedef struct {
	/**
	 * sender's user name
	 */
	char username[USER_NAME_SIZE];

	/**
	 * sender's user uuid
	 */
	uuid_t useruuid;

	/**
	 * chat room uuid
	 */
	uuid_t chatuuid;

	/**
	 * raw message
	 */
	char data[DATA_BUFFER_SIZE];

	PayloadMessageType type;
} PayloadMessage;

typedef struct {
	/**
	 * sender's user name
	 */
	char username[USER_NAME_SIZE];

	/**
	 * sender's user uuid
	 */
	uuid_t useruuid;
} PayloadUserInfo;

typedef struct {
	/**
	 * uuid for chat room
	 */
	uuid_t chatroomuuid;

	/**
	 * chatroom name
	 */
	char chatroomname[CHAT_ROOM_NAME_SIZE];
	
	/**
	 * total number of chatrooms on server
	 */
	int totalcount;

	/**
	 * sequence number of the chatroom out of the
	 * total chatroom
	 */
	int seqcount;
} PayloadChatInfo;

typedef struct {
	/**
	 * uuid for chat room
	 */
	uuid_t chatroomuuid;

	/**
	 * user that is enrolling
	 */
	uuid_t useruuid;
} PayloadChatEnrollment;

/**
 * Think of this like the form you need to fill out to 
 * enter a chatroom.
 */
typedef struct {
	/**
	 * user asking to join
	 */
	uuid_t useruuid;

	/**
	 * chatroom requested to join
	 */
	uuid_t chatroomuuid;

	/**
	 * 0 : request
	 * 1 : response
	 */
	unsigned char type;

	bool approved;

	/**
	 * contains public key for request and encrypted
	 * private key for response if approved
	 *
	 * This is not a clean way to do this. There is no
	 * standard as to how long a public/private key is based
	 * on type, as far as I know. This is here to implement functionality
	 */
	unsigned char data[2 << 10];

	// size of data above
	size_t datasize;

} PayloadChatroomEnrollmentForm;

typedef struct {
	struct {
		// struct version
		unsigned char version;
		
		/**
		 * the time this message was sent
		 */
		BFTime time;

		/**
		 * determines the payload
		 */
		PayloadType type;

		/**
		 * default 0xff to signify this
		 * packet is not part of a series of
		 * frames
		 *
		 * 0xfe will signify the last frame
		 */
		unsigned char frameno;
	} header;

	union {
		PayloadMessage message;
		PayloadUserInfo userinfo;
		PayloadChatInfo chatinfo;
		PayloadChatEnrollment enrollment;
		PayloadChatroomEnrollmentForm enrollform;
	} payload;
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

