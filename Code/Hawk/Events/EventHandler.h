#pragma once
#include <functional>

namespace Hawk {

struct IEvent;

class IEventHandler
{
public:
	virtual ~IEventHandler() = default;
	void _Call(const IEvent& p_Event) { Call(p_Event); }

private:
	virtual void Call(const IEvent&) = 0;
};

template<class T>
class EventHandler final : public IEventHandler
{
public:
	using Func = std::function<void(const T&)>;

	explicit EventHandler(Func& p_Func) : m_Func(p_Func) {}
	void Call(const IEvent& p_Event) override { m_Func(static_cast<const T&>(p_Event)); }

private:
	Func m_Func;
};
}