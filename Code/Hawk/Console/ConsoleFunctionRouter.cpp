#include "pch.h"
#include "Console/ConsoleFunctionRouter.h"
#include "System/Exception.h"

namespace Hawk {

void ConsoleFunctionRouter::Register(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	bool l_bInserted = m_ManagerMap.insert(ManagerMap_t::value_type(p_CmdName, std::weak_ptr<ModuleConsoleManager>(p_Manager))).second;
	THROW_IF_NOT(l_bInserted, "Failed to register console command for module. Command=" << p_CmdName);
}

void ConsoleFunctionRouter::Unregister(std::shared_ptr<ModuleConsoleManager>& p_Manager)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	for (auto& l_Itr = m_ManagerMap.begin(); l_Itr != m_ManagerMap.end(); )
	{
		auto l_Ptr = l_Itr->second.lock();
		LOG_IF(!l_Ptr, "ModuleConsoleManager destroyed without notification", "console", Fatal);

		if (l_Ptr == p_Manager)
		{
			l_Itr = m_ManagerMap.erase(l_Itr);
		}
		else
		{
			l_Itr++;
		}
	}
}

bool ConsoleFunctionRouter::TryGetManager(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager)
{
	auto l_Itr = m_ManagerMap.find(p_CmdName);
	if (l_Itr != m_ManagerMap.end())
	{
		p_Manager = l_Itr->second.lock();
		return p_Manager != nullptr;
	}
	else
	{
		LOG("No module has registered with console command: " << p_CmdName, "console", Info);
	}
	return false;
}

}