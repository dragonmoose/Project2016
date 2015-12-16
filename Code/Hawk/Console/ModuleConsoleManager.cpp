#include "pch.h"
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleManager.h"

namespace Hawk {

void ModuleConsoleManager::PushCommand(ConsoleCommand&& p_Command)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	m_CommandQueue.push_back(std::forward<ConsoleCommand>(p_Command));
}

void ModuleConsoleManager::ExecuteCommands()
{
	std::vector<ConsoleCommand> l_Commands;		// TODO: fix class for move?
	{
		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_CommandQueue.swap(l_Commands);
	}
	for (const auto& l_Command : l_Commands)
	{
		try
		{
			auto l_Itr = m_Functions.find(l_Command.GetName());
			THROW_IF(l_Itr == m_Functions.end(), "Failed to look up function");
			l_Itr->second->_Call(l_Command.GetArgs());
		}
		catch (Exception& e)
		{
			LOG("Failed to invoke function: " << l_Command.GetName() << " Exception: " << e.what(), "console", Error);
		}
	}
}

}
#endif