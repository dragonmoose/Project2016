#pragma once
#include "Events/EventCollector.h"
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <typeindex>
#include <vector>

namespace Hawk {

struct IEvent;

class HAWK_DLL_EXPORT EventRouter final
{
public:
	EventRouter() {}
	EventRouter(const EventRouter&) = delete;
	EventRouter& operator=(const EventRouter&) = delete;

	void Register(const std::type_index& p_EventTypeIndex, EventCollector* p_Collectors);
	bool TryGetCollectors(const std::type_index& p_TypeIndex, std::vector<EventCollector*>& p_EventManagers);

	template<class T>
	void Dispatch(const T& p_Event)
	{
		const std::type_index& l_TypeIndex = std::type_index(typeid(T));
		std::vector<EventCollector*> l_Collectors;
		if (TryGetCollectors(l_TypeIndex, l_Collectors))
		{
			for (EventCollector* l_Collector : l_Collectors)
			{
				l_Collector->Push<T>(p_Event);
			}
		}
	}

private:
	using FuncTable_t = std::unordered_map<std::type_index, std::vector<EventCollector*>>;
	FuncTable_t m_FuncTable;
};

}