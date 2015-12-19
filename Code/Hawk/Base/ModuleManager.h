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

class ModuleManager final
{
public:
	ModuleManager(const std::string& p_ThreadName);
	HAWK_DLL_EXPORT void Add(std::unique_ptr<Module> p_Module);
	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter);

#ifdef HAWK_DEBUG
	void SetConsoleCommandManager(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager);
#endif

	void Start();
	void Stop();

	ModuleManager(const ModuleManager&) = delete;
	ModuleManager& operator=(const ModuleManager&) = delete;

private:
	void Update();

	using Modules_t = std::vector<std::unique_ptr<Module>>;
	Modules_t m_Modules;
	Time m_OldTime;
	Thread m_Thread;

#ifdef HAWK_DEBUG
	std::shared_ptr<ConsoleCommandManager> m_ConsoleCommandManager;
#endif
};
}