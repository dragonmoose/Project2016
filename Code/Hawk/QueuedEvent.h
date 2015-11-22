#pragma once
#include "EventHandler.h"
#include <typeindex>

namespace Hawk {

class QueuedEventBase
{
public:
	QueuedEventBase(const std::type_index& p_TypeIndex) : m_TypeIndex(p_TypeIndex) {}
	virtual ~QueuedEventBase() {}

	void InternalCall(EventHandlerBase& p_EventHandler) { Call(p_EventHandler); }
	virtual void Call(EventHandlerBase& p_EventHandler) = 0;

	const std::type_index& GetTypeIndex() const { return m_TypeIndex; }

private:
	std::type_index m_TypeIndex;
};

template<class T>
class QueuedEvent final : public QueuedEventBase
{
public:
	QueuedEvent(const T& p_Event) : QueuedEventBase(std::type_index(typeid(T))), m_Event(p_Event) {}
	void Call(EventHandlerBase& p_EventHandler) override
	{
		p_EventHandler.InternalCall(m_Event);
	}

private:
	T m_Event;
};

}