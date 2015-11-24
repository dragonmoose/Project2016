#pragma once
#include "Hawk/SystemBase.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include "Hawk/Time.h"
#include "TestEvent.h"
#include <string>

class TestSystem2 : public Hawk::SystemBase
{
public:
	Hawk::Duration m_Duration;
	std::string GetName() const override { return "TestSystem2"; }
	void RegisterEvents(Hawk::EventManager& p_EventManager)
	{
		p_EventManager.Register<TestEvent>([this](const TestEvent& p_Event)
		{
			LOG_SYS("Receives event submitted by self", Debug);
		}
		);

		p_EventManager.Register<TestEvent2>([this](const TestEvent2& p_Event)
		{
			LOG_SYS("Value=" << p_Event.m_iValue, Debug);
		}
		);
	}
	void Update(const Hawk::Duration& p_Duration) override
	{
		m_Duration += p_Duration;
		if (m_Duration >= Hawk::Duration(1, Hawk::Duration::Precision::Second))
		{
			m_Duration.SetToZero();
			TestEvent l_Event;
			//l_Event.m_Str = "EventString";
			l_Event.m_iValue = 666;
			SendEvent<TestEvent>(l_Event);
		}
	}
};