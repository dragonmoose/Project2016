#include "pch.h"
#include "Module.h"
#include "Debug/Profiler.h"
#include "System/Duration.h"
#include "System/Exception.h"
#include <typeinfo>

namespace Hawk {

std::atomic_uint Module::s_uiNextModuleID = 1;

Module::Module()
: m_bPaused(false)
, m_ID(s_uiNextModuleID++)
{
}

Module::~Module()
{
#ifdef HAWK_DEBUG
	for (const std::string& l_Command : m_RegisteredConsoleCommands)
	{
		m_ConsoleCommandManager->Unregister(l_Command);
	}
#endif
}

ModuleID Module::GetID() const
{
	return m_ID;
}

void Module::_Initialize(std::unique_ptr<EventManager> p_EventManager, std::shared_ptr<Dispatcher>& p_Dispatcher)
{
	LOGM("Initializing module: " << GetName(), Info);
	m_Dispatcher = p_Dispatcher;
	m_EventManager = std::move(p_EventManager);
	Initialize();
	RegisterEvents(*m_EventManager);
}

void Module::Initialize()
{
}

#ifdef HAWK_DEBUG
void Module::_InitializeConsole(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager)
{
	m_ConsoleCommandManager = p_ConsoleCommandManager;
	InitializeConsole();
}

void Module::InitializeConsole()
{
}

std::string Module::GetLogDesc() const
{
	return GetName() + " #" + std::to_string(GetID());
}
#endif

void Module::RegisterEvents(EventManager& p_EventManager)
{
}

void Module::_Update(const Duration& p_Duration)
{
	if (!IsPaused())
	{
		if (!m_TimePerFrame.IsZero())
		{
			m_AccumulatedTime += p_Duration;
			if (m_AccumulatedTime >= m_TimePerFrame)
			{
				Profiler l_Profiler(GetName() + ":Update");

				l_Profiler.Start();
				m_EventManager->HandleQueued();
				Update(m_AccumulatedTime);
				l_Profiler.Stop();

				m_AccumulatedTime.SetToZero();
			}
		}
		else
		{
			Profiler l_Profiler(GetName() + ":Update");
			l_Profiler.Start();
			m_EventManager->HandleQueued();
			Update(p_Duration);
			l_Profiler.Stop();
		}
	}
	else
	{
		m_EventManager->ClearQueued();
	}
}

void Module::Update(const Duration& p_Duration)
{
}

void Module::SetPaused(bool p_bPaused)
{
	LOGM_IF(p_bPaused != m_bPaused, "Pause state changed. IsPaused=" << p_bPaused, Debug);
	m_bPaused = p_bPaused;
	m_AccumulatedTime.SetToZero();
}

bool Module::IsPaused() const
{
	return m_bPaused;
}

void Module::SetFixedTimeStep(float p_fValue, FixedTimeStepDecl p_Decl)
{
	float l_fValue = (p_Decl == FixedTimeStepDecl::FramesPerSecond ? (1.0f / p_fValue) : p_fValue);
	m_TimePerFrame = Duration(static_cast<int>(l_fValue * 1000000.0f), Duration::Precision::MicroSecond);
	LOGM("Using fixed time step. " << 1.0f / l_fValue << " FPS Interval: " << l_fValue << " seconds", Info);
}

}
