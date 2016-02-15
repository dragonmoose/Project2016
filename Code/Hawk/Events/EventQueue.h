#pragma once
#include "QueuedEvent.h"
#include "Threading/Mutex.h"
#include <vector>
#include <memory>

namespace Hawk {

class EventQueue final
{
public:
	EventQueue() : m_Mutex("EventQueue") {}
	EventQueue(const EventQueue&) = delete;
	EventQueue& operator=(const EventQueue&) = delete;

	using Queue_t = std::vector<std::unique_ptr<QueuedEventBase>>;

	template<class T>
	void Push(const T& p_Event)
	{
		std::unique_ptr<QueuedEvent<T>> l_QueuedEvent = std::make_unique<QueuedEvent<T>>(p_Event);
		MutexScope_t l_MutexScope(m_Mutex);
		m_Queue.push_back(std::move(l_QueuedEvent));
	}

	void Pop(Queue_t& p_Queue)
	{
		MutexScope_t l_MutexScope(m_Mutex);
		m_Queue.swap(p_Queue);
	}

	void Clear()
	{
		MutexScope_t l_MutexScope(m_Mutex);
		m_Queue.clear();
	}

private:
	Mutex m_Mutex;
	Queue_t m_Queue;
};

}