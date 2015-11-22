#include "pch.h"
#include "EventManager.h"

namespace Hawk {

void EventManager::HandleQueued()
{
	EventCollector::Queue_t l_QueuedEvents;
	m_Collector.Extract(l_QueuedEvents);
	while (!l_QueuedEvents.empty())
	{
		QueuedEventBase& l_Event = *l_QueuedEvents.back();
		auto l_Itr = m_Events.find(l_Event.GetTypeIndex());
		THROW_IF(l_Itr == m_Events.end(), "Failed to look up event handler for event: " << l_Event.GetTypeIndex().name());
		l_Event.InternalCall(*l_Itr->second);
		l_QueuedEvents.pop_back();
	}
}

}