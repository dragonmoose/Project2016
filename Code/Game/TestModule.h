#pragma once
#include "Hawk/Base/Module.h"
#include "Hawk/Console/Logger.h"
#include "Hawk/System/Duration.h"
#include "Hawk/Events/EventManager.h"
#include "TestEvent.h"
#include "Hawk/System/Duration.h"
#include "Hawk/Console/ConsoleFunctionManager.h"
#include <string>

class TestModule : public Hawk::Module
{
public:
	Hawk::Duration m_Duration;
	std::string GetName() const override { return "TestModule";  }
	virtual void Update(const Hawk::Duration& p_Duration)
	{
		m_Duration += p_Duration;
		if (m_Duration >= Hawk::Duration(500, Hawk::Duration::Precision::Millisecond))
		{
			m_Duration.SetToZero();
			TestEvent2 l_Event;
			//l_Event.m_Str = "fdsfsdfdsfsdfsdf";
			l_Event.m_iValue = 321321;
			//SendEvent<TestEvent2>(l_Event);
			//LOGM("testing testing the grejs", Fatal);
		}
	}

	void TestConsoleFunc(int a, int b)
	{

	}

	void RegisterConsole(Hawk::ConsoleFunctionManager& p_Manager) override
	{
		//p_Manager.Register("test.cmd", std::bind(&TestModule::TestConsoleFunc, this, std::placeholders::_1, std::placeholders::_2));
		//p_Manager.Register("test.cmd", [](int a, int b) {});
		//LOGM("reg class.", Interrupt);
	}

	void RegisterEvents(Hawk::EventManager& p_EventManager) override
	{
		p_EventManager.Register<TestEvent>([=](const TestEvent& p_Event)
		{
			LOGM("Event received. Value=" << p_Event.m_iValue, Debug);
		}
		);
	}
};
