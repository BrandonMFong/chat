/**
 * author: Brando
 * date: 3/12/24
 */

#ifndef AGENT_TESTS_HPP
#define AGENT_TESTS_HPP

#define ASSERT_PUBLIC_MEMBER_ACCESS

#include <bflibcpp/bflibcpp.hpp>
#include "agent.hpp"
#include "agentclient.hpp"
#include "agentserver.hpp"
#include "user.hpp"

extern "C" {
#include <bflibc/bflibc.h>
#include <bftest/bftest.h>
}

using namespace BF;

BFTEST_UNIT_FUNC(test_agents, 1, {
	AgentClient ac;
	AgentServer as;
})

BFTEST_UNIT_FUNC(test_agentuserrepresenationclient, 2<<8, {
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
})

BFTEST_COVERAGE_FUNC(agent_tests, {
	BFTEST_LAUNCH(test_agents);
	BFTEST_LAUNCH(test_agentuserrepresenationclient);
})

#endif // AGENT_TESTS_HPP

