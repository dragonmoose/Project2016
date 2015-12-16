#pragma once
#ifdef HAWK_DEBUG
#include "Console/ModuleConsoleManager.h"
#include "Console/ConsoleCommand.h"
#include "System/DllExport.h"
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>

namespace Hawk {

class HAWK_DLL_EXPORT ModuleConsoleRouter final
{
public:
	ModuleConsoleRouter() = default;
	ModuleConsoleRouter(const ModuleConsoleRouter&) = delete;
	ModuleConsoleRouter& operator=(const ModuleConsoleRouter&) = delete;

	void Register(const std::string& p_CmdName, std::shared_ptr<ModuleConsoleManager>& p_Manager);
	void Dispatch(ConsoleCommand&& p_Command);
	void ListRegistered();

private:
	using ManagerMap_t = std::unordered_map<std::string, std::weak_ptr<ModuleConsoleManager>>;
	ManagerMap_t m_ManagerMap;
	std::mutex m_Mutex;
};

}
#endif