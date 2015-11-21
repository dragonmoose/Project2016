#include "pch.h"
#include "EventRouter.h"

namespace Hawk {

void EventRouter::Register(const IEvent& p_Event, Func_t p_Func)
{
	m_FuncTable[std::type_index(typeid(p_Event))].push_back(p_Func);
}

void EventRouter::Dispatch(const IEvent& p_Event)
{
	auto l_Itr = m_FuncTable.find(std::type_index(typeid(p_Event)));
	if (l_Itr != m_FuncTable.end())
	{
		for (auto& l_PushFunc : l_Itr->second)
		{
			l_PushFunc(p_Event);
		}
	}
}

}