#include "pch.h"
#include "Module.h"
#include "SubModule.h"
#include "Debug/Assert.h"
#include "Debug/Profiler.h"
#include "System/Duration.h"
#include "System/Exception.h"
#include <typeinfo>

namespace Hawk {

std::atomic_uint Module::s_uiNextModuleID = 1;

Module::Module()
: m_bPaused(false)
, m_ID(s_uiNextModuleID++)
, m_bInitialized(false)
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

ModuleID_t Module::GetID() const
{
	return m_ID;
}

void Module::_Initialize(std::unique_ptr<EventManager> p_EventManager, std::shared_ptr<Dispatcher>& p_Dispatcher)
{
	try
	{
#ifdef HAWK_DEBUG
		m_LogDesc = GetName() + " #" + std::to_string(GetID());
#endif

		LOGM("Initializing module", Info);
		m_Dispatcher = p_Dispatcher;
		m_EventManager = std::move(p_EventManager);

		Initialize();
		for (auto& l_SubModule : m_SubModules)
		{
			l_SubModule->_Initialize();
			l_SubModule->RegisterEvents(*m_EventManager);
		}

		RegisterEvents(*m_EventManager);
		m_bInitialized = true;
	}
	catch (Exception& e)
	{
		LOGM_EXCEPTION(e, Fatal);
	}
	catch (std::exception& e)
	{
		LOGM_STD_EXCEPTION(e, Fatal);
	}
}

void Module::Initialize()
{
}

#ifdef HAWK_DEBUG
void Module::_InitializeConsole(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager)
{
	m_ConsoleCommandManager = p_ConsoleCommandManager;

	InitializeConsole();
	for (auto& l_SubModule : m_SubModules)
	{
		l_SubModule->InitializeConsole();
	}
}

void Module::InitializeConsole()
{
}

const std::string& Module::GetLogDesc() const
{
	return m_LogDesc;
}

void Module::DebugPrint() const
{
	std::cout << "\t" << GetLogDesc() << "\n";
	std::cout << "\t\t" << "Paused: " << IsPaused() << "\n";
	if (!m_SubModules.empty())
	{
		std::cout << "\t\tSubModules:\n";
		for (const auto& l_SubModule : m_SubModules)
		{
			std::cout << "\t\t\t" << l_SubModule->GetName() << "\n";
		}
	}
}
#endif

void Module::RegisterEvents(EventManager& p_EventManager)
{
}

void Module::_Update(const Duration& p_Duration)
{
	try
	{
		if (!IsPaused())
		{
			if (!m_TimePerFrame.IsZero())
			{
				m_AccumulatedTime += p_Duration;
				if (m_AccumulatedTime >= m_TimePerFrame)
				{
					Profiler l_Profiler(GetName() + ":FixedUpdate");

					l_Profiler.Start();
					m_EventManager->HandleQueued();

					Update(m_AccumulatedTime);
					for (auto& l_SubModule : m_SubModules)
					{
						l_SubModule->Update(m_AccumulatedTime);
					}

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
				for (auto& l_SubModule : m_SubModules)
				{
					l_SubModule->Update(p_Duration);
				}

				l_Profiler.Stop();
			}
		}
		else
		{
			m_EventManager->ClearQueued();
		}
	}
	catch (Exception& e)
	{
		LOGM_EXCEPTION(e, Error);
	}
	catch (std::exception& e)
	{
		LOGM_STD_EXCEPTION(e, Error);
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
	m_TimePerFrame = Duration(static_cast<int32_t>(l_fValue * 1000000.0f), Duration::Precision::MicroSecond);
	LOGM("Using fixed time step. " << 1.0f / l_fValue << " FPS Interval: " << l_fValue << " seconds", Debug);
}

void Module::AddSubModule(std::shared_ptr<SubModule> p_SubModule)
{
	ASSERT(!m_bInitialized, "Sub modules should be added prior to parent module initialization");
	ASSERT(std::find_if(m_SubModules.cbegin(), m_SubModules.cend(), [&p_SubModule](const std::shared_ptr<SubModule>& p_Other) { return p_SubModule->GetName() == p_Other->GetName(); }) == m_SubModules.cend(), "SubModule by the same name already added: " << p_SubModule->GetName());

	p_SubModule->SetParentModule(this);
	m_SubModules.push_back(p_SubModule);
}

bool Module::IsInitialized() const
{
	return m_bInitialized;
}

EventManager& Module::GetEventManager()
{
	return *m_EventManager.get();
}

}
