#pragma once
#include "EventQueue.h"
#include "Debug/Profiler.h"
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <typeindex>
#include <vector>
#include <memory>

namespace Hawk {

struct IEvent;

class HAWK_DLL_EXPORT EventRouter final
{
public:
	EventRouter() = default;
	EventRouter(const EventRouter&) = delete;
	EventRouter& operator=(const EventRouter&) = delete;

	using EventQueues_t = std::vector<std::weak_ptr<EventQueue>>;
	using EventQueuesMap_t = std::unordered_map<std::type_index, EventQueues_t>;

	void Register(const std::type_index& p_EventTypeIndex, std::shared_ptr<EventQueue>& p_EventQueue);

	template<class T>
	void Dispatch(const T& p_Event)
	{
		PROFILE();
		const std::type_index& l_TypeIndex = std::type_index(typeid(T));
		EventQueuesMap_t::iterator l_Itr = m_EventQueues.find(l_TypeIndex);
		if (l_Itr != m_EventQueues.end())
		{
			for (auto l_Itr2 = l_Itr->second.begin(); l_Itr2 != l_Itr->second.end(); )
			{
				std::shared_ptr<EventQueue> l_EventQueue = l_Itr2->lock();
				if (l_EventQueue)
				{
					l_EventQueue->Push<T>(p_Event);
					l_Itr2++;
				}
				else
				{
					l_Itr2 = l_Itr->second.erase(l_Itr2);
				}
			}
		}
	}

private:
	EventQueuesMap_t m_EventQueues;
};

}