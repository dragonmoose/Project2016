#pragma once
#ifdef HAWK_DEBUG
#include "ConsoleCommandManager.h"
#include <string>

namespace Hawk {

class Dispatcher;

class ScopedConsoleCommands final
{
public:
	ScopedConsoleCommands(std::shared_ptr<ConsoleCommandManager>& p_Manager, std::shared_ptr<Dispatcher>& p_Dispatcher);
	~ScopedConsoleCommands();

	ScopedConsoleCommands(const ScopedConsoleCommands&) = delete;
	ScopedConsoleCommands& operator=(const ScopedConsoleCommands&) = delete;

	template<class Object_t, class... Args_t>
	void Register(const std::string& p_Name, Object_t* p_Object, void(Object_t::*p_Func)(Args_t...), const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
	{
#ifdef HAWK_DEBUG
		m_Manager->Register(p_Name, p_Object, p_Func, m_Dispatcher.get(), p_Desc, p_ArgsDesc, p_bRequireArgs);
		m_Commands.push_back(p_Name);
#endif
	}

	template<class... Args_t>
	void Register(const std::string& p_Name, void(*p_Func)(Args_t...), const std::string& p_Desc, const std::string& p_ArgsDesc, bool p_bRequireArgs = true)
	{
#ifdef HAWK_DEBUG
		m_Manager->Register(p_Name, p_Func, m_Dispatcher.get(), p_Desc, p_ArgsDesc, p_bRequireArgs);
		m_Commands.push_back(p_Name);
#endif
	}

	void Unregister(const std::string& p_Name);

private:
	std::shared_ptr<ConsoleCommandManager> m_Manager;
	std::shared_ptr<Dispatcher> m_Dispatcher;
	std::vector<std::string> m_Commands;
};

}
#endif