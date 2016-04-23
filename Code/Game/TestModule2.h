#pragma once
#include "Hawk/Base/Module.h"
#include "Hawk/Console/Logger.h"
#include "Hawk/System/Duration.h"
#include "Hawk/System/Time.h"
#include "Hawk/Input/InputEvents.h"
#include "Hawk/Gfx/DebugEvents.h"
#include "TestEvent.h"
#include <string>

using namespace Hawk;

class TestModule2 : public Hawk::Module
{
public:
	Hawk::Duration m_Duration;
	std::string GetName() const override { return "TestModule2"; }
	void RegisterEvents(Hawk::EventManager& p_EventManager)
	{
		p_EventManager.Register<Hawk::InputEvents::KeyUpEvent>([this](const Hawk::InputEvents::KeyUpEvent& p_Event)
		{
			if (p_Event.m_Code == Hawk::KeyCode::A)
			{
				Hawk::Gfx::AddDebugAABBEvent l_Event(Vec3(0.0f), Vec3(1.0f));
				SendEvent(l_Event);
			}
			else if (p_Event.m_Code == Hawk::KeyCode::B)
			{
				Hawk::Gfx::AddDebugAABBEvent l_Event(Vec3(0.0f), Vec3(1.0f), Gfx::Color::Blue, 1, false, Duration(5, Duration::Precision::Second));
				SendEvent(l_Event);
			}

		}
		);

		p_EventManager.Register<TestEvent2>([this](const TestEvent2& /*p_Event*/)
		{
		}
		);

		p_EventManager.Register<TestEvent3>([this](const TestEvent3& /*p_Event*/)
		{
		}
		);
	}
	void Update(const Hawk::Duration& p_Duration) override
	{
		LOG("Tick: " << p_Duration.ToString() << " secflt: " << p_Duration.GetSecondsFloat(), "test", Trace);
	}

	void Initialize() override
	{
		SetFixedTimeStep(1, FixedTimeStepDecl::FramesPerSecond);
	}
};