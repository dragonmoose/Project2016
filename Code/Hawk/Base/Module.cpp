#include "pch.h"
#include "Base/Module.h"
#include "System/Duration.h"
#include <typeinfo>

namespace Hawk {

Module::Module()
: m_bPaused(false)
, m_Name(typeid(this).name())
{
}

Module::~Module()
{
}

void Module::InternalInitialize(std::unique_ptr<EventManager> p_EventManager)
{
	LOGM("Initializing module: " << GetName(), Info);
	m_EventManager = std::move(p_EventManager);
	Initialize();

#ifdef HAWK_DEBUG
	RegisterConsole(*m_ConsoleManager);
#endif
	RegisterEvents(*m_EventManager);
}

void Module::RegisterConsole(ModuleConsoleManager& p_ConsoleManager)
{
}

void Module::Initialize()
{
}

void Module::RegisterEvents(EventManager& p_EventManager)
{
}

void Module::InternalUpdate(const Duration& p_Duration)
{
#ifdef HAWK_DEBUG
	m_ConsoleManager->ExecuteCommands();
#endif
	if (!IsPaused())
	{
		m_EventManager->HandleQueued();
		Update(p_Duration);
	}
}

void Module::Update(const Duration& p_Duration)
{
}

void Module::UpdateConsole()
{
	m_ConsoleFunctionManager.
}

void Module::SetPaused(bool p_bPaused)
{
	m_bPaused = p_bPaused;
}
bool Module::IsPaused() const
{
	return m_bPaused;
}


}
