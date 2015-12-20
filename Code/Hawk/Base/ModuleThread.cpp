#include "pch.h"
#include "Base/ModuleThread.h"
#include "Base/Module.h"
#include "Time.h"
#include "System/Duration.h"
#include "Events/EventManager.h"
#include "Util/StringUtil.h"

namespace Hawk {

namespace { const char* c_Name("ModuleThread"); }

ModuleThread::ModuleThread(const std::string& p_Name)
: m_Thread(p_Name, std::bind(&ModuleThread::Update, this))
{
}

bool ModuleThread::TryRemove(ModuleID p_ID)
{
	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	Modules_t::const_iterator l_Itr = FindByID(p_ID);
	if (l_Itr != m_Modules.end())
	{
		m_Modules.erase(l_Itr);
		return true;
	}
	return false;
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

const std::string& ModuleThread::GetName() const
{
	return m_Thread.GetName();
}

ThreadID ModuleThread::GetThreadID() const
{
	return m_Thread.GetID();
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

	std::lock_guard<std::mutex> l_Lock(m_Mutex);
	for (auto& l_Module : m_Modules)
	{
		l_Module->_Update(l_Delta);
	}
}

ModuleThread::Modules_t::const_iterator ModuleThread::FindByID(ModuleID p_ID) const
{
	return std::find_if(m_Modules.begin(), m_Modules.end(), [p_ID](const std::unique_ptr<Module>& p_Module)
	{
		return p_ID == p_Module->GetID();
	});
}

}