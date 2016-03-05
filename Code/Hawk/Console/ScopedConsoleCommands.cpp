#include "pch.h"
#ifdef HAWK_DEBUG
#include "ScopedConsoleCommands.h"

namespace Hawk {

ScopedConsoleCommands::ScopedConsoleCommands(std::shared_ptr<ConsoleCommandManager>& p_Manager, std::shared_ptr<Dispatcher>& p_Dispatcher)
: m_Manager(p_Manager)
, m_Dispatcher(p_Dispatcher)
{
}

ScopedConsoleCommands::~ScopedConsoleCommands()
{
	for (const std::string& l_Command : m_Commands)
	{
		Unregister(l_Command);
	}
}

void ScopedConsoleCommands::Unregister(const std::string& p_Name)
{
	m_Manager->Unregister(p_Name);
}

}
#endif