#include "pch.h"
#include "SubModule.h"
#include "Module.h"

namespace Hawk {

SubModule::SubModule()
: m_pParentModule(nullptr)
{
}

void SubModule::SetParentModule(Module* p_pModule)
{
	m_pParentModule = p_pModule;
}

void SubModule::_Initialize()
{
	ASSERT(m_pParentModule, "Parent module not set");
	m_LogDesc = m_pParentModule->GetLogDesc() + ":" + GetName();
	Initialize();
}

void SubModule::Initialize()
{
}

#ifdef HAWK_DEBUG
void SubModule::InitializeConsole(ScopedConsoleCommands* /*p_Console*/)
{
}
#endif

const std::string& SubModule::GetLogDesc() const
{
	return m_LogDesc;
}

void SubModule::RegisterEvents(EventManager& /*p_EventManager*/)
{
}

void SubModule::Update(const Duration& /*p_Duration*/)
{
}

}
