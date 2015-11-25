#pragma once
#include "Hawk/Module.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include "Hawk/EventManager.h"
#include "TestEvent.h"
#include "Hawk/Duration.h"
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
			//LOG_MOD("testing testing the grejs", Fatal);
		}
	}

	void RegisterEvents(Hawk::EventManager& p_EventManager) override
	{
		p_EventManager.Register<TestEvent>([=](const TestEvent& p_Event)
		{
			LOG_MOD("Event received. Value=" << p_Event.m_iValue, Debug);
		}
		);
	}
};
