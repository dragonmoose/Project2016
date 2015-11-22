#pragma once
#include "QueuedEvent.h"
#include <vector>
#include <memory>
#include <mutex>

namespace Hawk {

class EventCollector final
{
public:
	typedef std::vector<std::unique_ptr<QueuedEventBase>> Queue_t;

	template<class T>
	void Push(const T& p_Event)
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Queue.push_back(std::make_unique<QueuedEvent<T>>(p_Event));
	}
	void Extract(Queue_t& p_Queue)
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Queue.swap(p_Queue);
	}
private:
	std::mutex m_Mutex;
	Queue_t m_Queue;
};

}