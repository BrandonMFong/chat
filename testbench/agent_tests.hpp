/**
 * author: Brando
 * date: 3/12/24
 */

#ifndef AGENT_TESTS_HPP
#define AGENT_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include "agent.hpp"
#include "agentclient.hpp"
#include "agentserver.hpp"
#include "user.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bflibc/bftests.h>
}

using namespace BF;

int test_agents() {
	UNIT_TEST_START;
	int result = 0;
	AgentClient ac;
	AgentServer as;
	
	UNIT_TEST_END(!result, result);
	return result;
}

int test_agentuserrepresenationclient() {
	UNIT_TEST_START;
	int result = 0;

	int max = 2 << 8;
	while (!result && max--) {
		AgentClient ac;
		List<User *> users;

		// create users
		int size = 50;
		for (int i = 0; i < size; i++) {
			uuid_t uuid;
			uuid_generate_random(uuid);
			User * user = new User("name", uuid);
			ac.setremoteuser(user);
			users.add(user);
		}

		uuid_t uuid;
		uuid_generate_random(uuid);
		if (ac.representsUserWithUUID(uuid)) {
			result = max;
		}

		if (!result) {
			users.first()->object()->getuuid(uuid);
			if (!ac.representsUserWithUUID(uuid)) {
				result = max;
			}
		}
	}	
	
	UNIT_TEST_END(!result, result);
	return result;
}

void agent_tests(int * pass, int * fail) {
	int p = 0, f = 0;
	
	INTRO_TEST_FUNCTION;

	LAUNCH_TEST(test_agents, p, f);
	LAUNCH_TEST(test_agentuserrepresenationclient, p, f);

	if (pass) *pass += p;
	if (fail) *fail += f;
}

#endif // AGENT_TESTS_HPP

