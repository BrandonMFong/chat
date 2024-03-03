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

	kPayloadTypeChatInfo = 5,
} PayloadType;

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
	} header;

	union {
		PayloadMessage message;
		PayloadUserInfo userinfo;
		PayloadChatInfo chatinfo;
	} payload;
} Packet;

#define PACKET_ALLOC (Packet *) malloc(sizeof(Packet))
#define PACKET_FREE(...) free(__VA_ARGS__)

#endif // TYPE_PACKET_H

