#include "pch.h"
#include "EventRouter.h"

namespace Hawk {

void EventRouter::Register(const std::type_index& p_EventTypeIndex, std::shared_ptr<EventQueue>& p_EventQueue)
{
	m_EventQueues[p_EventTypeIndex].push_back(p_EventQueue);
}

}