#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include "System/DllExport.h"
#include "System/Dispatcher.h"
#include "System/Exception.h"
#include "Events/EventRouter.h"
#include "Base/Module.h"
#include "Base/ModuleManager.h"
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

	void RegisterThread(const std::string& p_Name);

	template<class T>
	void AddModule(const std::string& p_Thread = std::string())
	{
		THROW_IF(p_Thread.empty(), "Empty thread name");
		auto l_Itr = m_ModuleManagers.find(p_Thread);
		THROW_IF(l_Itr == m_ModuleManagers.end(), "Thread with name " << p_Thread << " not registered");
		l_Itr->second->Add(Module::CreateInstance<T>());
	}
	void Run();

private:
	void InitializeModules();
	void StartModules();
	void StopModules();

	using ModuleManagers_t = std::unordered_map<std::string, std::unique_ptr<ModuleManager>>;
	ModuleManagers_t m_ModuleManagers;

	std::shared_ptr<EventRouter> m_EventRouter;
	static std::atomic_bool m_bFatalSignal;
	std::shared_ptr<Dispatcher> m_Dispatcher;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif

};

}