#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include "System/DllExport.h"
#include "System/Dispatcher.h"
#include "System/Exception.h"
#include "Events/EventRouter.h"
#include "Base/Module.h"
#include "Base/ModuleThread.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

class HAWK_DLL_EXPORT Core final
{
public:
	explicit Core(bool p_bConsole = true);
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	void OpenWindow(HINSTANCE p_hInstance, const std::string& p_Name);

	void CreateModuleThread(const std::string& p_Name);

	template<class T>
	void AddModule(const std::string& p_Name, const std::string& p_ThreadName)
	{
		THROW_IF(p_ThreadName.empty(), "Empty module thread name");
		ModuleThreads_t::iterator l_Itr = FindByThreadName(p_ThreadName);

		THROW_IF(l_Itr == m_ModuleThreads.end(), "No module thread named " << p_ThreadName << " exists");
		(**l_Itr).Add<T>(p_Name);
	}

	void RemoveModule(const std::string& p_Name, const std::string& p_ThreadName)
	{
		THROW_IF(p_ThreadName.empty(), "Empty module thread name");
		ModuleThreads_t::iterator l_Itr = FindByThreadName(p_ThreadName);

		THROW_IF(l_Itr == m_ModuleThreads.end(), "No module thread named " << p_ThreadName << " exists");
		(**l_Itr).Remove(p_Name);
	}
	void Run();

private:
	using ModuleThreads_t = std::vector<std::unique_ptr<ModuleThread>>;

	void InitializeModules();
	void StartModules();
	void StopModules();

	ModuleThreads_t::iterator FindByThreadName(const std::string& p_Name);

#ifdef HAWK_DEBUG
	void RegisterConsole();
#endif

	ModuleThreads_t m_ModuleThreads;

	std::shared_ptr<EventRouter> m_EventRouter;
	static std::atomic_bool m_bFatalSignal;
	std::shared_ptr<Dispatcher> m_Dispatcher;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif

};

}