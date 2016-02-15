#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include "CoreSettings.h"
#include "Module.h"
#include "ModuleThread.h"
#include "System/DllExport.h"
#include "Threading/Dispatcher.h"
#include "System/Exception.h"
#include "Events/EventRouter.h"
#include "System/Types.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

class HAWK_DLL_EXPORT Core final
{
public:
	Core(const CoreSettings& p_Settings = CoreSettings());
	~Core();
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	void Initialize();
	void OpenWindow(HINSTANCE p_hInstance, const std::string& p_Name);

	ThreadID CreateModuleThread(const std::string& p_Name);

	template<class Module_t, class... Args_t>
	ModuleID AddModule(ThreadID p_ThreadID, Args_t&&... p_Args)
	{
		THROW_IF(p_ThreadID == ThreadID_Invalid, "Invalid thread id");

		ModuleThread* l_ModuleThread = nullptr;
		THROW_IF_NOT(TryGetModuleThread(p_ThreadID, &l_ModuleThread), "No thread with ID " << p_ThreadID << " exists");
		return l_ModuleThread->Add<Module_t>(std::forward<Args_t>(p_Args)...);
	}

	void RemoveModule(ModuleID p_ID);
	void SetPaused(ModuleID p_ID, bool p_bPaused);
	void Run();

private:
	using ModuleThreads_t = std::vector<std::unique_ptr<ModuleThread>>;

	void AddModules();
	void InitializeModules();
	void StartModules();
	void StopModules();

	bool ModuleThreadExists(const std::string& p_Name) const;
	bool TryGetModuleThread(ThreadID p_ThreadID, ModuleThread** p_ModuleThread) const;
	bool TryGetModule(ModuleID p_ID, Module** p_Module) const;

#ifdef HAWK_DEBUG
	void RegisterConsole();
	void CmdListModules();
#endif

	ModuleThreads_t m_ModuleThreads;

	std::shared_ptr<EventRouter> m_EventRouter;
	static std::atomic_bool m_bFatalSignal;
	CoreSettings m_Settings;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
	std::shared_ptr<Dispatcher> m_DebugDispatcher;
#endif

};

}