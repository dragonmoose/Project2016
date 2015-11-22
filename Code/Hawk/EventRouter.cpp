#include "pch.h"
#include "EventRouter.h"

namespace Hawk {

namespace EventRouter
{
	typedef std::unordered_map<std::type_index, std::vector<EventCollector*>> FuncTable_t;
	FuncTable_t m_FuncTable;
}

void EventRouter::Register(const std::type_index& p_EventTypeIndex, EventCollector* p_Func)
{
	m_FuncTable[p_EventTypeIndex].push_back(p_Func);
}

bool EventRouter::TryGetCollectors(const std::type_index& p_TypeIndex, std::vector<EventCollector*>& p_EventManagers)
{
	FuncTable_t::iterator l_Itr = m_FuncTable.find(p_TypeIndex);
	if (l_Itr != m_FuncTable.end())
	{
		p_EventManagers = l_Itr->second;
		return true;
	}
	return false;
}

}