#include "pch.h"
#include "EventManager.h"
#include "EventRouter.h"

namespace Hawk {

EventManager::EventManager(std::shared_ptr<EventRouter> p_Router)
: m_Router(p_Router)
{
#ifdef EVENT_MANAGER_LOCKFREE
	THROW_IF_NOT(m_Queue.is_lock_free(), "Queue is not lock-free");
#endif
}

void EventManager::HandleQueued()
{
#ifdef EVENT_MANAGER_LOCKFREE
	IEvent l_Event;
	while (m_Queue.pop(l_Event))
	{
		const std::type_index& l_Index = std::type_index(typeid(l_Event));
		auto l_Itr = m_Events.find(l_Index);
		THROW_IF(l_Itr == m_Events.end(), "Failed to look up event handler for event: " << l_Index.name());
		l_Itr->second->InternalCall(l_Event);
	}
#else
	std::vector<IEvent> l_Events;
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Queue.swap(l_Events);
	}
	while (!l_Events.empty())
	{
		const IEvent& l_Event = m_Queue.back();
		const std::type_index& l_Index = std::type_index(typeid(l_Event));
		auto l_Itr = m_Events.find(l_Index);
		THROW_IF(l_Itr == m_Events.end(), "Failed to look up event handler for event: " << l_Index.name());
		l_Itr->second->InternalCall(l_Event);
		m_Queue.pop_back();
	}
#endif
}

void EventManager::Send(IEvent& p_Event)
{
	m_Router->Dispatch(p_Event);
}

void EventManager::Push(const IEvent& p_Event)
{
#ifdef EVENT_MANAGER_LOCKFREE
	THROW_IF_NOT(m_Queue.push(p_Event), "Failed to push event to queue");
#else
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_Queue.push_back(p_Event);
#endif
}

}