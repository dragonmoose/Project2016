#include "pch.h"
#include "Base/ModuleThread.h"
#include "Base/Module.h"
#include "Time.h"
#include "System/Duration.h"
#include "Events/EventManager.h"

namespace Hawk {

namespace { const char* c_Name("ModuleThread"); }

ModuleThread::ModuleThread(const std::string& p_ThreadName)
: m_Thread(p_ThreadName, std::bind(&ModuleThread::Update, this))
{
}

void ModuleThread::Add(std::unique_ptr<Module> p_Module)
{
	m_Modules.push_back(std::move(p_Module));
}

void ModuleThread::Initialize(std::shared_ptr<EventRouter>& p_EventRouter)
{
	for (auto& l_Module : m_Modules)
	{
		l_Module->_Initialize(std::make_unique<EventManager>(p_EventRouter), m_Thread.GetDispatcher());
#if HAWK_DEBUG
		l_Module->_InitializeConsole(m_ConsoleCommandManager);
#endif
	}
}

#if HAWK_DEBUG
void ModuleThread::SetConsoleCommandManager(std::shared_ptr<ConsoleCommandManager>& p_ConsoleCommandManager)
{
	m_ConsoleCommandManager = p_ConsoleCommandManager;
}
#endif

void ModuleThread::Start()
{
	LOG("Running " << m_Modules.size() << " modules on thread: " << m_Thread.GetName(), c_Name, Info);
	m_OldTime.SetToNow();
	m_Thread.Start();
}

void ModuleThread::Stop()
{
	m_Thread.Stop();
}

void ModuleThread::Update()
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