#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleFunction.h"
#include "Console/ConsoleCommand.h"
#include "Console/Logger.h"
#include "System/DllExport.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>

namespace Hawk {

class ModuleConsoleManager final
{
public:
	ModuleConsoleManager() = default;
	ModuleConsoleManager(const ModuleConsoleManager&) = delete;
	ModuleConsoleManager& operator=(const ModuleConsoleManager&) = delete;

	template<class Ret_t, class ObjPtr_t, class... Args_t>
	void Register(const std::string& p_Name, ObjPtr_t* p_ObjPtr, Ret_t(ObjPtr_t::*p_Func)(Args_t...))
	{
		bool l_bInserted = m_Functions.insert(Functions_t::value_type(p_Name, std::make_unique<ConsoleFunction<Ret_t, ObjPtr_t, Args_t...>>(p_ObjPtr, p_Func))).second;
		THROW_IF_NOT(l_bInserted, "Failed to register console function (already registered?): " << p_Name);
	}

	void PushCommand(ConsoleCommand&& p_Command);
	void ExecuteCommands();

private:
	using Functions_t = std::unordered_map<std::string, std::unique_ptr<ConsoleFunctionBase>>;
	Functions_t m_Functions;

	using CommandQueue_t = std::vector<ConsoleCommand>;
	CommandQueue_t m_CommandQueue;
	std::mutex m_Mutex;
};

}
#endif