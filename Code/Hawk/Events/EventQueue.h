#pragma once
#include "Events/QueuedEvent.h"
#include <vector>
#include <memory>
#include <mutex>

namespace Hawk {

class EventQueue final
{
public:
	EventQueue() {}
	EventQueue(const EventQueue&) = delete;
	EventQueue& operator=(const EventQueue&) = delete;

	using Queue_t = std::vector<std::unique_ptr<QueuedEventBase>>;

	template<class T>
	void Push(const T& p_Event)
	{
		std::unique_ptr<QueuedEvent<T>> l_QueuedEvent = std::make_unique<QueuedEvent<T>>(p_Event);
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Queue.push_back(std::move(l_QueuedEvent));
	}

	void Pop(Queue_t& p_Queue)
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Queue.swap(p_Queue);
	}
private:
	std::mutex m_Mutex;
	Queue_t m_Queue;
};

}