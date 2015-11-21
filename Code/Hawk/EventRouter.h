#pragma once
#include <unordered_map>
#include <typeindex>

namespace Hawk {

struct IEvent;

class EventRouter final
{
public:
	typedef std::function<void(const IEvent&)> Func_t;

	void Register(const IEvent& p_Event, Func_t p_Func);
	void Dispatch(const IEvent& p_Event);

private:
	typedef std::unordered_map<std::type_index, std::vector<Func_t>> FuncTable_t;
	FuncTable_t m_FuncTable;
};

}