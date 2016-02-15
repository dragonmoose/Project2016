#pragma once
#include "System/DllExport.h"
#include "System/Duration.h"
#include "Threading/Mutex.h"
#include "Threading/Thread.h"
#include "System/Time.h"
#include "System/Types.h"
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

	template<class Module_t, class... Args_t>
	ModuleID Add(Args_t&&... p_Args)
	{
		std::unique_ptr<Module> l_Module = std::make_unique<Module_t>(std::forward<Args_t>(p_Args)...);

		ModuleID l_ModuleID = l_Module->GetID();

		MutexScope_t l_MutexScope(m_Mutex);
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
	void OnFrameBegin();
	void OnFrameEnd();
	Modules_t::const_iterator FindByID(ModuleID p_ID) const;

	Modules_t m_Modules;
	Time m_CurrFrameStartTime;
	Time m_PrevFrameStartTime;
	Thread m_Thread;
	mutable Mutex m_Mutex;
	static const Duration sc_MaxFrameDuration;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif
};
}