#include "pch.h"
#include "Base/Module.h"
#include "System/Duration.h"
#include "System/Exception.h"
#include <typeinfo>

namespace Hawk {

Module::Module()
: m_bPaused(false)
{
}

void Module::SetName(const std::string& p_Name)
{
	THROW_IF(p_Name.empty(), "Name is empty");
	m_Name = p_Name;
}

const std::string& Module::GetName() const
{
	return m_Name;
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
}

void Module::Update(const Duration& p_Duration)
{
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
