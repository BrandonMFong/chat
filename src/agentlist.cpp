/**
 * author: brando
 * date: 2/26/24
 */

#include "agentlist.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "agent.hpp"

using namespace BF;

Atomic<List<Agent *>> agents;

void _AgentListReleaseAgent(Agent * a) {
	Delete(a);
}

int AgentList::AddAgent(Agent * a) {
	agents.lock();

	// if agents are 0, then we can safely assume
	// the callback hasn't been set yet
	if (agents.unsafeget().count() == 0) {
		agents.unsafeget().setDeallocateCallback(_AgentListReleaseAgent);
	}

	agents.unsafeget().add(a);
	agents.unlock();
	return 0;
}

