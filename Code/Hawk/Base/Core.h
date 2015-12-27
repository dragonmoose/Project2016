#pragma once
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include "System/DllExport.h"
#include "System/Dispatcher.h"
#include "System/Exception.h"
#include "Events/EventRouter.h"
#include "System/Types.h"
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

	ThreadID CreateModuleThread(const std::string& p_Name);

	template<class T>
	ModuleID AddModule(ThreadID p_ThreadID)
	{
		THROW_IF(p_ThreadID == ThreadID_Invalid, "Invalid thread id");

		ModuleThread* l_ModuleThread = nullptr;
		THROW_IF_NOT(TryGetModuleThread(p_ThreadID, &l_ModuleThread), "No thread with ID " << p_ThreadID << " exists");
		return l_ModuleThread->Add<T>();
	}

	void RemoveModule(ModuleID p_ID);
	void SetPaused(ModuleID p_ID, bool p_bPaused);
	void Run();

private:
	using ModuleThreads_t = std::vector<std::unique_ptr<ModuleThread>>;

	void InitializeModules();
	void StartModules();
	void StopModules();

	bool TryGetModuleThread(ThreadID p_ThreadID, ModuleThread** p_ModuleThread) const;
	bool TryGetModule(ModuleID p_ID, Module** p_Module) const;

#ifdef HAWK_DEBUG
	void RegisterConsole();
	void CmdListModules();
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