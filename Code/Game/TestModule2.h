#pragma once
#include "Hawk/Base/Module.h"
#include "Hawk/Console/Logger.h"
#include "Hawk/System/Duration.h"
#include "Hawk/System/Time.h"
#include "TestEvent.h"
#include <string>

class TestModule2 : public Hawk::Module
{
public:
	Hawk::Duration m_Duration;
	std::string GetName() const override { return "TestModule2"; }
	void RegisterEvents(Hawk::EventManager& p_EventManager)
	{
		p_EventManager.Register<TestEvent>([this](const TestEvent& p_Event)
		{
			LOGM("Receives event submitted by self", Debug);
		}
		);

		p_EventManager.Register<TestEvent2>([this](const TestEvent2& p_Event)
		{
			LOGM("Value=" << p_Event.m_iValue, Debug);
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