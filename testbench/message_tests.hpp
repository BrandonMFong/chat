/**
 * author: Brando
 * date: 2/15/24
 */

#ifndef MESSAGE_TESTS_HPP
#define MESSAGE_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "message.hpp"
#include "packet.hpp"
#include "cipher.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_messageinit() {
	UNIT_TEST_START;
	int result = 0;

	Packet p;
	Message * m = new Message(&p);
	Delete(m);

	UNIT_TEST_END(!result, result);
	return result;
}

void _test_messagesLoadRandomDataToMessagePayload(Packet * p, const char * message) {
	memset(p, 0, sizeof(Packet));

	uuid_t uuidu, uuidc;
	uuid_generate_random(uuidu);
	uuid_generate_random(uuidc);

	PacketPayloadSetPayloadMessage(
		&p->payload.message,
		kPayloadMessageTypeData,
		uuidc, "username", uuidu,
		message);
}

int test_packet2message() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max) {
		Packet p;
		_test_messagesLoadRandomDataToMessagePayload(&p, "hello world");

		Message * m = new Message(&p);
		if (!m)
			result = max;

		if (!result) {
			uuid_t u0, u1;
			m->getuuidchatroom(u0);
			m->getuuiduser(u1);
			if (strcmp(m->data(), p.payload.message.data)) {
				result = max;
			} else if (strcmp(m->username(), p.payload.message.username)) {
				result = max;
			} else if (uuid_compare(u0, p.payload.message.chatuuid)) {
				result = max;
			} else if (uuid_compare(u1, p.payload.message.useruuid)) {
				result = max;
			} else if (m->type() != kPayloadMessageTypeData) {
				result = max;
			}
		}

		Delete(m);

		max--;
	}

	UNIT_TEST_END(!result, result);
	return result;

}

int test_messageEncryptAndDecrypt() {
	UNIT_TEST_START;
	int result = 0;
	int max = 1;

	while (!result && max--) {
		Packet p;
		String str = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
		_test_messagesLoadRandomDataToMessagePayload(&p, str.cString());
		Message * m = new Message(&p);
		if (!m) {
			result = 1;
			break;
		}

		Cipher * cipher = Cipher::create(kCipherTypeSymmetric);
		if (!cipher) {
			result = 2;
			break;
		} else if (cipher->genkey()) {
			result = 3;
			break;
		}

		if (m->encryptData(cipher)) {
			result = 3;
			break;
		}

		if (m->decryptData(cipher)) {
			result = 4;
			break;
		}

		Delete(m);
		Delete(cipher);
	}

	UNIT_TEST_END(!result, result);
	return result;
}

void message_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_messageinit, p, f);
	LAUNCH_TEST(test_packet2message, p, f);
	LAUNCH_TEST(test_messageEncryptAndDecrypt, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // MESSAGE_TESTS_HPP

