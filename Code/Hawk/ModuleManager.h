#pragma once
#include "DllExport.h"
#include "EventRouter.h"
#include <memory>
#include <thread>
#include <vector>
#include <atomic>

namespace Hawk {

class Module;
class ConsoleManager;

class ModuleManager final
{
public:
	ModuleManager(const std::string& p_ThreadName);
	HAWK_DLL_EXPORT void Add(std::unique_ptr<Module> p_Module);
	void Initialize(std::shared_ptr<EventRouter>& p_EventRouter, ConsoleManager& p_ConsoleManager);
	void Start();
	void Stop();

	ModuleManager(const ModuleManager&) = delete;
	ModuleManager& operator=(const ModuleManager&) = delete;

private:
	void Run_Thread();
	void Update(const Duration& p_Duration);

	using Modules_t = std::vector<std::unique_ptr<Module>>;
	Modules_t m_Modules;
	std::thread m_Thread;
	const std::string m_ThreadName;
	std::atomic_bool m_bStopSignal;
};
}