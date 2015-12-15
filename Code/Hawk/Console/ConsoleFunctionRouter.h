#pragma once
#include "Console/ModuleConsoleManager.h"
#include <string>
#include <unordered_map>
#include <mutex>

namespace Hawk {

class ConsoleFunctionRouter final
{
public:
	ConsoleFunctionRouter(const ConsoleFunctionRouter&) = delete;
	ConsoleFunctionRouter& operator=(const ConsoleFunctionRouter&) = delete;

	void Register(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager);
	void Unregister(std::shared_ptr<ModuleConsoleManager>& p_Manager);
	bool TryGetManager(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager);

private:
	using ManagerMap_t = std::unordered_map<std::string, std::weak_ptr<ModuleConsoleManager>>;
	ManagerMap_t m_ManagerMap;
	std::mutex m_Mutex;
};

}