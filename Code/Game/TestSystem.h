#pragma once
#include "Hawk/SystemBase.h"
#include "Hawk/Logger.h"
#include "Hawk/Duration.h"
#include "Hawk/EventManager.h"
#include "TestEvent.h"
#include <string>

class TestSystem : public Hawk::SystemBase
{
public:
	std::string GetName() const override { return "TestSystem";  }
	virtual void Update(const Hawk::Duration& p_Duration) { /*LOG_DEBUG("TestSystem update called");*/ }

	void RegisterEvents(Hawk::EventManager& p_EventManager) override
	{
		p_EventManager.Register<TestEvent>([](const TestEvent& p_Event)
		{
			LOG_DEBUG("Event received. Str=" << p_Event.m_Str << " Value=" << p_Event.m_iValue);
		}
		);
	}
};
