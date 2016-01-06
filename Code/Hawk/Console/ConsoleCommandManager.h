#pragma once
#ifdef HAWK_DEBUG
#include "ConsoleFunction.h"
#include "Logger.h"
#include "System/Exception.h"
#include "Util/StringUtil.h"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
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

			std::lock_guard<std::mutex> l_Lock(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(l_Name, std::make_unique<CF::MemberConsoleFunction<Object_t, Args_t...>>(p_Object, p_Func, p_Dispatcher, l_Name, p_Desc, p_ArgsDesc, p_bRequireArgs))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register member console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "console", Fatal);
		}
	}

	template<class... Args_t>
	void Register(const std::string& p_Name, void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher, const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
	{
		try
		{
			THROW_IF_NOT(p_Dispatcher, "NULL dispatcher");

			const std::string l_Name = StringUtil::ToLower(p_Name);

			std::lock_guard<std::mutex> l_Lock(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(l_Name, std::make_unique<CF::FreeConsoleFunction<Args_t...>>(p_Func, p_Dispatcher, l_Name, p_Desc, p_ArgsDesc, p_bRequireArgs))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register free console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "console", Fatal);
		}
	}

	void Unregister(const std::string& p_Name);

private:
	void Write(const std::string& p_Line, bool p_bNewLine) const;
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

	std::string GetNextCommand(const std::string& p_Command, const std::string& p_Current) const;
	void TryCallFunction(const CF::IConsoleFunction& p_Function, ConsoleInputParser& p_Command) const;

	std::thread m_Thread;
	std::atomic_bool m_bStopSignal;
	std::shared_ptr<Dispatcher> m_Dispatcher;

	mutable std::mutex m_Mutex;
	using FunctionMap_t = std::map<std::string, std::unique_ptr<CF::IConsoleFunction>>;
	FunctionMap_t m_Functions;
};
}
#endif