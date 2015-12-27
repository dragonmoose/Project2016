#pragma once
#include "System/DllExport.h"
#include "System/Thread.h"
#include "System/Time.h"
#include "System/Types.h"
#include "Events/EventRouter.h"
#ifdef HAWK_DEBUG
#include "Console/ConsoleCommandManager.h"
#endif
#include <memory>
#include <vector>
#include <mutex>

namespace Hawk {

class Module;

class HAWK_DLL_EXPORT ModuleThread final
{
public:
	ModuleThread(const std::string& p_Name);

	template<class T>
	ModuleID Add()
	{
		std::unique_ptr<Module> l_Module = std::make_unique<T>();

		ModuleID l_ModuleID = l_Module->GetID();

		std::lock_guard<std::mutex> l_Lock(m_Mutex);
		m_Modules.push_back(std::move(l_Module));
		return l_ModuleID;
	}

	bool TryRemove(ModuleID p_ID);
	bool TryGetModule(ModuleID p_ID, Module** p_Module) const;

	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);
	const std::string& GetName() const;
	ThreadID GetThreadID() const;

#ifdef HAWK_DEBUG
	void SetConsoleCommandManager(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager);
	void DebugPrint();
#endif

	void Start();
	void Stop();

	ModuleThread(const ModuleThread&) = delete;
	ModuleThread& operator=(const ModuleThread&) = delete;

private:
	using Modules_t = std::vector<std::unique_ptr<Module>>;

	void Update();
	Modules_t::const_iterator FindByID(ModuleID p_ID) const;

	Modules_t m_Modules;
	Time m_OldTime;
	Thread m_Thread;
	mutable std::mutex m_Mutex;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif
};
}