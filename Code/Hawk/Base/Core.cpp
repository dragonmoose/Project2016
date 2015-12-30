#include "pch.h"
#include "Base/Core.h"
#include "Base/WindowManager.h"
#include "Console/ConsoleAPI.h"
#include "System/Duration.h"
#include "System/Time.h"
#include "System/Thread.h"
#include "Util/StringUtil.h"
#include <boost/filesystem.hpp>
#include <thread>
#include <chrono>

namespace Hawk {

namespace { const char* c_Name("Core"); }

Core::Core(bool p_bConsole)
: m_EventRouter(std::make_shared<EventRouter>())
, m_Dispatcher(std::make_shared<Dispatcher>())
#ifdef HAWK_DEBUG
, m_ConsoleCommandManager(std::make_shared<ConsoleCommandManager>(m_Dispatcher))
#endif
{
#ifdef HAWK_DEBUG
	boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
	if (p_bConsole)
	{
		ConsoleAPI::Start();
		Logger::RegisterThread(Thread::MainThreadName, std::this_thread::get_id());
		m_ConsoleCommandManager->Start();
		RegisterConsole();
	}
	Config::Instance().Load(true);
	LOG("Working directory set to: " << boost::filesystem::current_path(), c_Name, Info);
#endif
	WindowManager::Initialize(m_EventRouter);
	LOG("Hawk core initialized...", c_Name, Info);
}

ThreadID Core::CreateModuleThread(const std::string& p_Name)
{
	THROW_IF(ModuleThreadExists(p_Name), "A thread has already been created with name: " << p_Name);

	std::unique_ptr<ModuleThread> l_ModuleThread = std::make_unique<ModuleThread>(p_Name);
	ThreadID l_ThreadID = l_ModuleThread->GetThreadID();
	m_ModuleThreads.push_back(std::move(l_ModuleThread));

	LOG("Thread created. Name=" << p_Name << " ID=" << l_ThreadID, c_Name, Info);
	return l_ThreadID;
}

void Core::OpenWindow(HINSTANCE p_hInstance, const std::string& p_Name)
{
	try
	{
		WindowManager::Open(p_hInstance, p_Name);
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION(e, c_Name, Fatal);
	}
}

void Core::RemoveModule(ModuleID p_ID)
{
	THROW_IF(p_ID == ModuleID_Invalid, "Invalid module ID");
	for (auto& l_ModuleThread : m_ModuleThreads)
	{
		if (l_ModuleThread->TryRemove(p_ID))
		{
			LOG("Removed module with ID=" << p_ID << " from thread=" << l_ModuleThread->GetThreadID(), "core", Debug);
			return;
		}
	}
	THROW("Failed to remove module with ID=" << p_ID << " (not found");
}

void Core::SetPaused(ModuleID p_ID, bool p_bPaused)
{
	Module* l_Module = nullptr;
	if (TryGetModule(p_ID, &l_Module))
	{
		l_Module->SetPaused(p_bPaused);
	}
}

void Core::Run()
{
	InitializeModules();
	StartModules();

	while (!Logger::FatalFlagSet())
	{
		Config::Instance().Update();
		if (!WindowManager::Update())
		{
			LOG("MainWindow signalled WM_QUIT", c_Name, Info);
			break;
		}
		m_Dispatcher->Execute();
	}
	StopModules();
#ifdef HAWK_DEBUG
	m_ConsoleCommandManager->Stop();
#endif

	LOG("************* Core exit *************", c_Name, Info);
	if (Logger::FatalFlagSet())
	{
		FATAL("Core exit due to critical error (see above)", c_Name);
	}
	std::this_thread::sleep_for(std::chrono::minutes(1));
	ConsoleAPI::Stop();
}

void Core::InitializeModules()
{
	for (auto& l_Manager : m_ModuleThreads)
	{
#ifdef HAWK_DEBUG
		l_Manager->SetConsoleCommandManager(m_ConsoleCommandManager);
#endif
		l_Manager->Initialize(m_EventRouter);
	}
}

void Core::StartModules()
{
	for (auto& l_Manager : m_ModuleThreads)
	{
		l_Manager->Start();
	}
}

void Core::StopModules()
{
	for (auto& l_Manager : m_ModuleThreads)
	{
		l_Manager->Stop();
	}
}

bool Core::ModuleThreadExists(const std::string& p_Name) const
{
	auto l_Itr = std::find_if(m_ModuleThreads.cbegin(), m_ModuleThreads.cend(), [p_Name](const std::unique_ptr<ModuleThread>& p_ModuleThread)
	{
		return StringUtil::AreEqual(p_Name, p_ModuleThread->GetName());
	});
	return l_Itr != m_ModuleThreads.cend();
}

bool Core::TryGetModuleThread(ThreadID p_ThreadID, ModuleThread** p_ModuleThread) const
{
	auto l_Itr = std::find_if(m_ModuleThreads.cbegin(), m_ModuleThreads.cend(), [p_ThreadID](const std::unique_ptr<ModuleThread>& p_ModuleThread)
	{
		return p_ThreadID == p_ModuleThread->GetThreadID();
	});

	if (l_Itr != m_ModuleThreads.cend())
	{
		*p_ModuleThread = l_Itr->get();
		return true;
	}
	return false;
}

bool Core::TryGetModule(ModuleID p_ID, Module** p_Module) const
{
	for (const auto& l_ModuleThread : m_ModuleThreads)
	{
		if (l_ModuleThread->TryGetModule(p_ID, p_Module))
		{
			return true;
		}
	}
	return false;
}

#ifdef HAWK_DEBUG
void Core::RegisterConsole()
{
	m_ConsoleCommandManager->Register("module.remove", this, &Core::RemoveModule, m_Dispatcher.get(), "Removes the specified module.", "[moduleID]");
	m_ConsoleCommandManager->Register("module.setPaused", this, &Core::SetPaused, m_Dispatcher.get(), "Pause/resume module.", "[mouleID] [0/1]");
	m_ConsoleCommandManager->Register("module.list", this, &Core::CmdListModules, m_Dispatcher.get(), "Lists modules", "");
}

void Core::CmdListModules()
{
	CONSOLE_WRITE_SCOPE();
	for (const auto& l_ModuleThread : m_ModuleThreads)
	{
		l_ModuleThread->DebugPrint();
	}
}

#endif
	
}