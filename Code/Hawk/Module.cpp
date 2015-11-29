#include "pch.h"
#include "Module.h"
#include "Duration.h"
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

void Module::InternalInitialize(std::unique_ptr<EventManager> p_EventManager, ConsoleManager& p_ConsoleManager)
{
	LOGM("Initializing module: " << GetName(), Info);
	m_EventManager = std::move(p_EventManager);
	Initialize();
	RegisterConsole(p_ConsoleManager);
	RegisterEvents(*m_EventManager);
}

void Module::Initialize()
{
}

void Module::RegisterEvents(EventManager& p_EventManager)
{
}

void Module::RegisterConsole(ConsoleManager& p_ConsoleManager)
{

}

void Module::InternalUpdate(const Duration& p_Duration)
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
