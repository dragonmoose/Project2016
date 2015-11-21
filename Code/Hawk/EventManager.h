#pragma once
#include "IEvent.h"
#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <memory>
#include <string>
#include <functional>
#include <type_traits>

#ifndef EVENT_MANAGER_LOCKFREE
#include <mutex>
#endif

#include "boost/lockfree/queue.hpp"

namespace Hawk {

class EventRouter;

class EventManager final
{
public:
	EventManager(std::shared_ptr<EventRouter> p_Router);

	template<class T>
	void Register(std::function<void(const T&)> p_Func)
	{
		static_assert(std::is_trivially_constructible<T>::value, "EventManager: Event needs to be trivially constructible");
		static_assert(std::is_trivially_destructible<T>::value, "EventManager: Event needs to be trivially destructible");
		static_assert(std::is_trivially_assignable<T&, const T&>::value, "EventManager: Event needs to be trivially assignable");
		m_Events[std::type_index(typeid(T))].push_back(std::make_unique<EventHandler<T>>(p_Func));
	}

	void HandleQueued();
	void Send(IEvent& p_Event);
	void Push(const IEvent& p_Event);
	
private:
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

	typedef std::unordered_map<std::type_index, std::unique_ptr<EventHandlerBase>> EventHandlerMap_t;
	EventHandlerMap_t m_Events;

#ifdef EVENT_MANAGER_LOCKFREE
	typedef boost::lockfree::queue<IEvent> Queue_t;
	Queue_t m_Queue;
#else
	std::mutex m_Mutex;
	typedef std::vector<IEvent> Queue_t;
	Queue_t m_Queue;
#endif
	std::shared_ptr<EventRouter> m_Router;
};

}

/*class TestSystem
{
public:
	TestSystem()
	{
		m_EventManager.Register<TestEvent>(std::bind(&TestSystem::Func, this, std::placeholders::_1));
	}
	void Func(const TestEvent& p_Event)
	{
		std::cout << "Func() in TestSystem called. Value=" << p_Event.m_iValue << " String=" << p_Event.m_Name << "\n";
	}
	void Update()
	{
		// Fetch events from global queue
	}
	EventManager m_EventManager;
};

class TestSystem2
{
public:
	TestSystem2()
	{
		m_EventManager.Register<TestEvent>(std::bind(&TestSystem2::Func, this, std::placeholders::_1));
	}
	void Func(const TestEvent& p_Event)
	{
		std::cout << "Func() in TestSystem2 called. Value=" << p_Event.m_iValue << " String=" << p_Event.m_Name << "\n";
	}
	void Update()
	{
		// For each system, collect out tray:s
		// Distribute to in trays
	}
	EventManager m_EventManager;
};

class Core
{
public:
	Core(TestSystem& s1, TestSystem2& s2) : m_s1(s1), m_s2(s2) {}
	void Update()
	{
		//m_s1.m_EventManager.m_PostedEvents.GetPosted();
		//m_s2.m_EventManager.m_PostedEvents.GetPosted();
	}
	TestSystem& m_s1;
	TestSystem2& m_s2;
};

int TEST_EVENT_SYS()
{
	LOG_INFO("Test of event sys");

	EventManager l_EventManager;
	TestSystem l_TestSystem;
	TestSystem2 l_TestSystem2;

	Core l_Core(l_TestSystem, l_TestSystem2);

	int i = 0;
	std::string l_Str = "grejertillbåten";
	while (1)
	{
		using namespace std::literals;
		std::this_thread::sleep_for(3s);

		TestEvent l_Event;
		l_Event.m_iValue = i++;
		l_Event.m_Name = l_Str;
		l_EventManager.Send(l_Event);
		i++;

		l_Core.Update();
		l_TestSystem.Update();
		l_TestSystem2.Update();
	}*/