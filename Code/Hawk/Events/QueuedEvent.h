#pragma once
#include "EventHandler.h"
#include <typeindex>

namespace Hawk {

class QueuedEventBase
{
public:
	explicit QueuedEventBase(const std::type_index& p_TypeIndex) : m_TypeIndex(p_TypeIndex) {}
	virtual ~QueuedEventBase() {}

	void _Call(IEventHandler& p_EventHandler) { Call(p_EventHandler); }
	virtual void Call(IEventHandler& p_EventHandler) = 0;

	const std::type_index& GetTypeIndex() const { return m_TypeIndex; }

private:
	std::type_index m_TypeIndex;
};

template<class T>
class QueuedEvent final : public QueuedEventBase
{
public:
	explicit QueuedEvent(const T& p_Event) : QueuedEventBase(std::type_index(typeid(T))), m_Event(p_Event) {}
	void Call(IEventHandler& p_EventHandler) override
	{
		p_EventHandler._Call(m_Event);
	}

private:
	T m_Event;
};

}