#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleRouter.h"
#include "System/Exception.h"

namespace Hawk {

void ModuleConsoleRouter::Register(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	bool l_bInserted = m_ManagerMap.insert(ManagerMap_t::value_type(p_CmdName, std::weak_ptr<ModuleConsoleManager>(p_Manager))).second;
	THROW_IF_NOT(l_bInserted, "Failed to register console command for module. Command=" << p_CmdName);
}

void ModuleConsoleRouter::Dispatch(ConsoleCommand&& p_Command)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	auto l_Itr = m_ManagerMap.find(p_Command.GetName());
	THROW_IF(l_Itr == m_ManagerMap.end(), "Unknown console command: " << p_Command.GetName());

	std::shared_ptr<ModuleConsoleManager> l_Manager = l_Itr->second.lock();
	if (l_Manager)
	{
		l_Manager->PushCommand(std::forward<ConsoleCommand>(p_Command));
	}
	else
	{
		m_ManagerMap.erase(l_Itr);
		LOG("Removed ModuleConsoleManager from map", "console", Debug);
	}
}

void ModuleConsoleRouter::ListRegistered()
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	for (const auto& l_Value : m_ManagerMap)
	{
		std::cout << l_Value.first << "\n";
	}
	std::cout << "\n";
}

}
#endif