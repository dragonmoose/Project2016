#include "pch.h"
#include "Events/EventManager.h"

namespace Hawk {

EventManager::EventManager(std::shared_ptr<EventRouter>& p_Router)
: m_Router(p_Router)
, m_EventQueue(std::make_shared<EventQueue>())
{
}

void EventManager::HandleQueued()
{
	EventQueue::Queue_t l_QueuedEvents;
	m_EventQueue->Pop(l_QueuedEvents);
	while (!l_QueuedEvents.empty())
	{
		QueuedEventBase& l_Event = *l_QueuedEvents.back();
		auto l_Itr = m_Events.find(l_Event.GetTypeIndex());
		THROW_IF(l_Itr == m_Events.end(), "Failed to look up event handler for event: " << l_Event.GetTypeIndex().name());
		l_Event._Call(*l_Itr->second);
		l_QueuedEvents.pop_back();
	}
}

void EventManager::ClearQueued()
{
	m_EventQueue->Clear();
}

}