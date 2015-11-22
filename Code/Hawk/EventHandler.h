#pragma once
#include <functional>

namespace Hawk {

struct IEvent;

class EventHandlerBase
{
public:
	virtual ~EventHandlerBase() {}
	void InternalCall(const IEvent& p_Event) { Call(p_Event); }

private:
	virtual void Call(const IEvent&) = 0;
};

template<class T>
class EventHandler final : public EventHandlerBase
{
public:
	typedef std::function<void(const T&)> Func_t;

	explicit EventHandler(Func_t& p_Func) : m_Func(p_Func) {}
	void Call(const IEvent& p_Event) override { m_Func(static_cast<const T&>(p_Event)); }

private:
	Func_t m_Func;
};
}