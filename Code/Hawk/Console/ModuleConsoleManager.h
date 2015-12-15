#pragma once
#include "Console/ConsoleFunction.h"
#include "Console/ConsoleFunctionRouter.h"
#include "Console/ConsoleCommand.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace Hawk {

class ModuleConsoleManager final : std::enable_shared_from_this<ModuleConsoleManager>
{
public:
	~ModuleConsoleManager();
	ModuleConsoleManager(const ModuleConsoleManager&) = delete;
	ModuleConsoleManager& operator=(const ModuleConsoleManager&) = delete;

	template<class Ret_t, class ModulePtr_t, class... Args_t>
	void Register(const std::string& p_Name, ModulePtr_t* p_ModulePtr, Ret_t(ModulePtr_t::*p_Func)(Args_t...))
	{
		bool l_bInserted = m_Functions.push_back(Functions_t::value_type(p_Name, std::make_unique<ConsoleFunction<Ret_t, p_ModulePtr, Args_t...>>(p_ModulePtr, p_Func))).second;
		LOG_IF(!l_bInserted, "Failed to register console function (already registered?): " << p_Name, "console", Fatal);
	}

	void PushCommand(const ConsoleCommand& p_Command);
	void ExecuteCommands();

private:
	std::shared_ptr<ConsoleFunctionRouter> m_Router;

	using Functions_t = std::unordered_map<std::string, std::unique_ptr<ConsoleFunctionBase>>;
	Functions_t m_Functions;

	using CommandQueue_t = std::vector<ConsoleCommand>;
	CommandQueue_t m_CommandQueue;
	std::mutex m_Mutex;
};

}