#pragma once
#include "Hawk/Base/Module.h"
#include "Hawk/Console/Logger.h"
#include "Hawk/System/Duration.h"
#include "Hawk/Events/EventManager.h"
#include "TestEvent.h"
#include "Hawk/System/Duration.h"
#include "Hawk/Gfx/DebugEvents.h"
#include <string>

class TestModule : public Hawk::Module
{
public:
	Hawk::Duration m_Duration;
	std::string GetName() const override { return "TestModule"; }

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
		}
	}

	void TestConsoleCmd()
	{
		Hawk::Gfx::UpdateDebugTextEvent l_Ev("value", "1234", "gfx");
		SendEvent(l_Ev);
		Hawk::Gfx::UpdateDebugTextEvent l_Ev2("test", "123fdf", "test");
		SendEvent(l_Ev2);
		Hawk::Gfx::UpdateDebugTextEvent l_Ev3("ffgf", "123wererw", "gfx");
		SendEvent(l_Ev3);
		Hawk::Gfx::UpdateDebugTextEvent l_Ev4("erw", "123fsdfd", "test");
		SendEvent(l_Ev4);
	}

	void TestConsoleCmd2()
	{
		Hawk::Gfx::RemoveDebugTextEvent l_Ev("value", "gfx");
		SendEvent(l_Ev);
		Hawk::Gfx::RemoveDebugTextEvent l_Ev2("test", "test");
		SendEvent(l_Ev2);
		Hawk::Gfx::RemoveDebugTextEvent l_Ev3("ffgf", "gfx");
		SendEvent(l_Ev3);
		Hawk::Gfx::RemoveDebugTextEvent l_Ev4("aerw", "test");
		SendEvent(l_Ev4);
	}

	void InitializeConsole()
	{
		RegisterConsole("test.cmd", this, &TestModule::TestConsoleCmd, "Test command", "[Text] [num1] [num2]");
		RegisterConsole("test.rem", this, &TestModule::TestConsoleCmd2, "Sends test event", "[Text]");
	}

	void RegisterEvents(Hawk::EventManager& p_EventManager) override
	{
		p_EventManager.Register<TestEvent>([=](const TestEvent& p_Event)
		{
			LOGM("Event received. Value=" << p_Event.m_iValue, Debug);


		}
		);
	}
	void Initialize() override
	{
		//SetFixedUpdate(30.0f, FixedUpdateType::FramesPerSecond);
	}
};
