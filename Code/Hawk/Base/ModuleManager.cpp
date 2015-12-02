#include "pch.h"
#include "Base/ModuleManager.h"
#include "Base/Module.h"
#include "Time.h"
#include "System/Duration.h"
#include "Events/EventManager.h"

namespace Hawk {

namespace { const char* c_Name("ModuleManager"); }

ModuleManager::ModuleManager(const std::string& p_ThreadName)
: m_ThreadName(p_ThreadName)
, m_bStopSignal(false)
{
}

void ModuleManager::Add(std::unique_ptr<Module> p_Module)
{
	m_Modules.push_back(std::move(p_Module));
}

void ModuleManager::Initialize(std::shared_ptr<EventRouter>& p_EventRouter, ConsoleCmdManager& p_ConsoleCmdManager)
{
	for (auto& l_Module : m_Modules)
	{
		l_Module->InternalInitialize(std::make_unique<EventManager>(p_EventRouter), p_ConsoleCmdManager);
	}
}

void ModuleManager::Start()
{
	m_Thread = std::thread(&ModuleManager::Run_Thread, this);
#ifdef HAWK_DEBUG
	Logger::RegisterThreadName(m_ThreadName, m_Thread.get_id());
#endif
}

void ModuleManager::Stop()
{
	m_bStopSignal = true;
	m_Thread.join();
}

void ModuleManager::Update(const Duration& p_Duration)
{
	for (auto& l_Module : m_Modules)
	{
		l_Module->InternalUpdate(p_Duration);
	}
}

void ModuleManager::Run_Thread()
{
	LOG("Running " << m_Modules.size() << " modules on thread: " << m_ThreadName, c_Name, Info);

	Time l_OldTime;
	l_OldTime.SetToNow();
	while (Config::Instance().Get<bool>("Core.run", true) && !m_bStopSignal)
	{
		Time l_CurrTime;
		l_CurrTime.SetToNow();
		Duration l_Delta(l_CurrTime - l_OldTime);
		l_OldTime = l_CurrTime;

		Update(l_Delta);
	}
	LOG("*** Thread exit ***", c_Name, Debug);
}

}