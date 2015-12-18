#include "pch.h"
#include "Base/ModuleManager.h"
#include "Base/Module.h"
#include "Time.h"
#include "System/Duration.h"
#include "Events/EventManager.h"

namespace Hawk {

namespace { const char* c_Name("ModuleManager"); }

ModuleManager::ModuleManager(const std::string& p_ThreadName)
: m_Thread(p_ThreadName, std::bind(&ModuleManager::Update, this))
{
}

void ModuleManager::Add(std::unique_ptr<Module> p_Module)
{
	m_Modules.push_back(std::move(p_Module));
}

void ModuleManager::Initialize(std::shared_ptr<EventRouter>& p_EventRouter)
{
	for (auto& l_Module : m_Modules)
	{
#if HAWK_DEBUG
		l_Module->_InitializeConsole(m_ConsoleRouter);
#endif
		l_Module->_Initialize(std::make_unique<EventManager>(p_EventRouter));
	}
}

#if HAWK_DEBUG
void ModuleManager::SetConsoleRouter(std::shared_ptr<ModuleConsoleRouter>& p_ConsoleRouter)
{
	m_ConsoleRouter = p_ConsoleRouter;
}
#endif

void ModuleManager::Start()
{
	LOG("Running " << m_Modules.size() << " modules on thread: " << m_Thread.GetName(), c_Name, Info);
	m_OldTime.SetToNow();
	m_Thread.Start();
}

void ModuleManager::Stop()
{
	m_Thread.Stop();
}

void ModuleManager::Update()
{
	Time l_CurrTime;
	l_CurrTime.SetToNow();
	Duration l_Delta(l_CurrTime - m_OldTime);
	m_OldTime = l_CurrTime;

	for (auto& l_Module : m_Modules)
	{
		l_Module->_Update(l_Delta);
	}
}

}