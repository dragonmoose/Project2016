#pragma once
#include "System/DllExport.h"
#include "System/Thread.h"
#include "System/Time.h"
#include "Events/EventRouter.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include <memory>
#include <vector>

namespace Hawk {

class Module;

class HAWK_DLL_EXPORT ModuleThread final
{
public:
	ModuleThread(const std::string& p_Name);

	template<class T>
	void Add(const std::string& p_Name)
	{
		Modules_t::const_iterator l_Itr = FindByName(p_Name);
		THROW_IF_NOT(l_Itr == m_Modules.end(), "Module thread already contains a module named " << p_Name);

		std::unique_ptr<T> l_Module = std::make_unique<T>();
		l_Module->SetName(p_Name);
		m_Modules.push_back(std::move(l_Module));
	}

	void Remove(const std::string& p_Name);

	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);
	const std::string& GetName() const;

#ifdef HAWK_DEBUG
	void SetConsoleCommandManager(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager);
#endif

	void Start();
	void Stop();

	ModuleThread(const ModuleThread&) = delete;
	ModuleThread& operator=(const ModuleThread&) = delete;

private:
	using Modules_t = std::vector<std::unique_ptr<Module>>;

	void Update();
	Modules_t::const_iterator FindByName(const std::string& p_Name) const;

	Modules_t m_Modules;
	Time m_OldTime;
	Thread m_Thread;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif
};
}