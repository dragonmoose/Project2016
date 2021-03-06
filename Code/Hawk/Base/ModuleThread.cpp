#include "pch.h"
#include "ModuleThread.h"
#include "Module.h"
#include "Events/EventManager.h"
#include "System/Duration.h"
#include "Util/StringUtil.h"
#include <iomanip>

namespace Hawk {

const Duration ModuleThread::sc_MaxFrameDuration(1, Duration::Precision::Second);

ModuleThread::ModuleThread(const std::string& p_Name)
: m_Thread(p_Name, std::bind(&ModuleThread::Update, this))
, m_Mutex(p_Name)
{
}

bool ModuleThread::TryRemove(ModuleID p_ID)
{
	MutexScope l_MutexScope(m_Mutex);
	Modules::const_iterator l_Itr = FindByID(p_ID);
	if (l_Itr != m_Modules.end())
	{
		m_Modules.erase(l_Itr);
		return true;
	}
	return false;
}

bool ModuleThread::TryGetModule(ModuleID p_ID, Module** p_Module) const
{
	MutexScope l_MutexScope(m_Mutex);
	Modules::const_iterator l_Itr = FindByID(p_ID);
	if (l_Itr != m_Modules.end())
	{
		*p_Module = l_Itr->get();
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

void ModuleThread::DebugPrint()
{
	std::cout << "[" << GetName() << " #" << GetThreadID() << "]\n";
	for (const auto& l_Module : m_Modules)
	{
		l_Module->DebugPrint();
	}
	std::cout << "\n";
}
#endif

void ModuleThread::Start()
{
	LOG("Running " << m_Modules.size() << " modules on thread: " << m_Thread.GetName(), "core", Info);
	m_PrevFrameStartTime.SetToNow();
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

	Duration l_Delta(l_CurrTime - m_PrevFrameStartTime);
	l_Delta = std::min(l_Delta, sc_MaxFrameDuration);

	Profiler l_Profiler(std::string("MTUpdate:").append(m_Thread.GetName()));
	l_Profiler.Start();

	MutexScope l_MutexScope(m_Mutex);
	for (auto& l_Module : m_Modules)
	{
		l_Module->_Update(l_Delta);
	}
	m_PrevFrameStartTime = l_CurrTime;
}

ModuleThread::Modules::const_iterator ModuleThread::FindByID(ModuleID p_ID) const
{
	return std::find_if(m_Modules.begin(), m_Modules.end(), [p_ID](const std::unique_ptr<Module>& p_Module)
	{
		return p_ID == p_Module->GetID();
	});
}

}