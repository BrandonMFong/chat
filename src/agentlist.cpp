/**
 * author: brando
 * date: 2/26/24
 */

#include "agentlist.hpp"
#include <bflibcpp/bflibcpp.hpp>
#include "agent.hpp"

using namespace BF;

Atomic<List<Agent *>> agents;

int AgentList::AddAgent(Agent * a) {
	return 0;
}

