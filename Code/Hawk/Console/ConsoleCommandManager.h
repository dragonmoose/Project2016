#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleFunction.h"
#include "Console/Logger.h"
#include "System/Dispatcher.h"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <thread>
#include <unordered_map>

namespace Hawk {

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
	void Register(const std::string& p_Name, Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), Dispatcher* p_Dispatcher)
	{
		try
		{
			THROW_IF_NOT(p_Dispatcher, "NULL dispatcher");

			std::lock_guard<std::mutex> l_Lock(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(p_Name, std::make_unique<MemberConsoleFunction<Object_t, Args_t...>>(p_Object, p_Func, p_Dispatcher))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register member console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "console", Fatal);
		}
	}

	template<class... Args_t>
	void Register(const std::string& p_Name, void(*p_Func)(Args_t...), Dispatcher* p_Dispatcher)
	{
		try
		{
			THROW_IF_NOT(p_Dispatcher, "NULL dispatcher");

			std::lock_guard<std::mutex> l_Lock(m_Mutex);
			bool l_bInserted = m_Functions.insert(FunctionMap_t::value_type(p_Name, std::make_unique<FreeConsoleFunction<Args_t...>>(p_Func, p_Dispatcher))).second;
			THROW_IF_NOT(l_bInserted, "Failed to register free console function (already registered?): " << p_Name);
		}
		catch (Exception& e)
		{
			LOG_EXCEPTION(e, "console", Fatal);
		}
	}

private:
	void RunInputLoop();

	std::thread m_Thread;
	std::atomic_bool m_bStopSignal;
	std::shared_ptr<Dispatcher> m_Dispatcher;

	mutable std::mutex m_Mutex;
	using FunctionMap_t = std::unordered_map<std::string, std::unique_ptr<IConsoleFunction>>;
	FunctionMap_t m_Functions;
};
}
#endif