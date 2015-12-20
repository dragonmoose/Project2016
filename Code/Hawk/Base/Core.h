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
		ModuleThreads_t::iterator l_Itr = FindByThreadID(p_ThreadID);

		THROW_IF(l_Itr == m_ModuleThreads.end(), "No thread with ID " << p_ThreadID << " exists");
		return (**l_Itr).Add<T>();
	}

	void RemoveModule(ModuleID p_ID)
	{
		THROW_IF(p_ID == ModuleID_Invalid, "Invalid module ID");
		for (auto& l_ModuleThread : m_ModuleThreads)
		{
			if (l_ModuleThread->TryRemove(p_ID))
			{
				LOG("Removed module with ID=" << p_ID << " from thread=" << l_ModuleThread->GetThreadID(), "core", Debug);
				return;
			}
		}
		THROW("Failed to remove module with ID=" << p_ID << " (not found");
	}
	void Run();

private:
	using ModuleThreads_t = std::vector<std::unique_ptr<ModuleThread>>;

	void InitializeModules();
	void StartModules();
	void StopModules();

	ModuleThreads_t::iterator FindByThreadID(ThreadID p_ThreadID);

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