#pragma once
#ifdef HAWK_DEBUG
#include "ConsoleFunction.h"
#include "ConsoleHistory.h"
#include "Logger.h"
#include "System/Exception.h"
#include "Threading/Mutex.h"
#include "Threading/Thread.h"
#include "Util/StringUtil.h"
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <map>

namespace Hawk {

class Dispatcher;
class ConsoleInputParser;

class ConsoleCommandManager final
{
public:
	using InputLines_t = std::vector<std::string>;

	ConsoleCommandManager(std::shared_ptr<Dispatcher>& p_Dispatcher);
	ConsoleCommandManager(const ConsoleCommandManager&) = delete;
	ConsoleCommandManager& operator=(const ConsoleCommandManager&) = delete;

	void Start();
	void Stop();

	template<class Object_t, class... Args_t>
	void Register(const std::string& p_Name, Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
	{
		try
		{
			THROW_IF_NOT(p_Dispatcher, "NULL dispatcher");

			const std::string l_Name = StringUtil::ToLower(p_Name);

			MutexScope_t l_MutexScope(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(l_Name, std::make_unique<CF::MemberConsoleFunction<Object_t, Args_t...>>(p_Object, p_Func, p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register member console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			FATAL_EXCEPTION(e, "console");
		}
	}

	template<class... Args_t>
	void Register(const std::string& p_Name, void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
	{
		try
		{
			THROW_IF_NOT(p_Dispatcher, "NULL dispatcher");

			const std::string l_Name = StringUtil::ToLower(p_Name);

			MutexScope_t l_MutexScope(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(l_Name, std::make_unique<CF::FreeConsoleFunction<Args_t...>>(p_Func, p_Dispatcher, p_Name, p_Desc, p_ArgsDesc, p_bRequireArgs))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register free console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			FATAL_EXCEPTION(e, "console");
		}
	}

	void Unregister(const std::string& p_Name);

private:
	void RunInputLoop();

	void Register();
	void CmdQuit();
	void CmdListCommands(const std::string& p_Filter = std::string());
	void CmdGetConfig(const std::string& p_Key);
	void CmdReloadConfig();
	void CmdToggleLog();
	void CmdSetLogLevel(const std::string& p_Level);
	void CmdSetLogFilter(const std::string& p_Filter);
	void CmdSetLogThread(const std::string& p_Filter);
	void CmdSetLogTag(const std::string& p_Filter);
	void CmdPrintHistory();
	void CmdClearHistory();
	void CmdAbout();

	std::string GetNextAutoCompletedCmd(const std::string& p_Command, const std::string& p_Current) const;
	void TryCallFunction(const CF::IConsoleFunction& p_Function, ConsoleInputParser& p_Command) const;

	Thread m_Thread;
	std::shared_ptr<Dispatcher> m_Dispatcher;

	std::string m_CurrLine;
	std::string m_CurrTypedLine;

	mutable Mutex m_Mutex;
	using FunctionMap_t = std::map<std::string, std::unique_ptr<CF::IConsoleFunction>>;
	FunctionMap_t m_Functions;

	std::unique_ptr<ConsoleHistory> m_History;
	std::string m_Prompt;
};
}
#endif