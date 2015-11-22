#pragma once
#include "EventCollector.h"
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <typeindex>
#include <vector>

namespace Hawk {

struct IEvent;

namespace EventRouter
{
	HAWK_DLL_EXPORT void Register(const std::type_index& p_EventTypeIndex, EventCollector* p_Collectors);
	HAWK_DLL_EXPORT bool TryGetCollectors(const std::type_index& p_TypeIndex, std::vector<EventCollector*>& p_EventManagers);

	template<class T>
	void Dispatch(const T& p_Event)
	{
		const std::type_index& l_TypeIndex = std::type_index(typeid(T));
		//LOG_INFO("hash: " << p_EventTypeIndex.hash_code() << " name: " << p_EventTypeIndex.name());
		std::vector<EventCollector*> l_Collectors;
		if (TryGetCollectors(l_TypeIndex, l_Collectors))
		{
			for (EventCollector* l_Collector : l_Collectors)
			{
				l_Collector->Push<T>(p_Event);
			}
		}
	}
}

}