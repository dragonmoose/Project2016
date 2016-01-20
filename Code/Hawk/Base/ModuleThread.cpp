#include "pch.h"
#include "ModuleThread.h"
#include "Module.h"
#include "Events/EventManager.h"
#include "System/Duration.h"
#include "Util/StringUtil.h"
#include <iomanip>

namespace Hawk {

ModuleThread::ModuleThread(const std::string& p_Name)
: m_Thread(p_Name, std::bind(&ModuleThread::Update, this))
, m_Mutex(p_Name)
{
	m_Thread.RegisterFrameBegin(std::bind(&ModuleThread::OnFrameBegin, this));
	m_Thread.RegisterFrameEnd(std::bind(&ModuleThread::OnFrameEnd, this));
}

bool ModuleThread::TryRemove(ModuleID p_ID)
{
	MutexScope_t l_MutexScope(m_Mutex);
	Modules_t::const_iterator l_Itr = FindByID(p_ID);
	if (l_Itr != m_Modules.end())
	{
		m_Modules.erase(l_Itr);
		return true;
	}
	return false;
}

bool ModuleThread::TryGetModule(ModuleID p_ID, Module** p_Module) const
{
	MutexScope_t l_MutexScope(m_Mutex);
	Modules_t::const_iterator l_Itr = FindByID(p_ID);
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
	Profiler l_Profiler(std::string("MTUpdate:").append(m_Thread.GetName()));
	l_Profiler.Start();
	Duration l_Delta(m_CurrFrameStartTime - m_PrevFrameStartTime);
	MutexScope_t l_MutexScope(m_Mutex);
	for (auto& l_Module : m_Modules)
	{
		l_Module->_Update(l_Delta);
	}
}

void ModuleThread::OnFrameBegin()
{
	m_CurrFrameStartTime = Time::Now();
}

void ModuleThread::OnFrameEnd()
{
	m_PrevFrameStartTime = m_CurrFrameStartTime;
}

ModuleThread::Modules_t::const_iterator ModuleThread::FindByID(ModuleID p_ID) const
{
	return std::find_if(m_Modules.begin(), m_Modules.end(), [p_ID](const std::unique_ptr<Module>& p_Module)
	{
		return p_ID == p_Module->GetID();
	});
}

}