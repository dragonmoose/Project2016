#include "pch.h"
#include "Base/Module.h"
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
		m_EventManager->HandleQueued();
		Update(p_Duration);
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
}

bool Module::IsPaused() const
{
	return m_bPaused;
}


}
