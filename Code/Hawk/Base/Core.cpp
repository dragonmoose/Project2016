#include "pch.h"
#include "Core.h"
#include "WindowManager.h"
#include "Debug/ProfilerManager.h"
#include "Gfx/RenderingModule.h"
#include "Scene/SceneManager.h"
#include "System/Time.h"
#include "Threading/Thread.h"
#include "Threading/ThreadInfoManager.h"
#include "Util/Util_Console.h"
#include "Util/Random.h"
#include "Util/StringUtil.h"
#include <boost/filesystem.hpp>
#include <thread>
#include <chrono>

namespace Hawk {

Core::Core(const CoreSettings& p_Settings)
: m_Settings(p_Settings)
{
}

Core::~Core()
{
#ifdef HAWK_DEBUG
	if (m_Settings.m_bConsole)
	{
		m_ConsoleCommandManager->Stop();

		LOG("************* Core exit *************", "core", Info);
		if (Logger::FatalFlagSet())
		{
			LOG("Core exit due to critical error (see above)", "core", Fatal);
		}

		INT32 l_iExitWaitTimeSec = Config::Instance().Get("dev.shutdownDelaySec", 30);
		if (l_iExitWaitTimeSec > 0)
		{
			LOG("Waiting " << l_iExitWaitTimeSec << " seconds before shutting down console...", "core", Info);
			std::this_thread::sleep_for(std::chrono::seconds(l_iExitWaitTimeSec));
		}
		ConsoleAPI::Stop();
	}
#endif
}

void Core::Initialize()
{
	m_EventRouter = std::make_shared<EventRouter>();
	boost::filesystem::current_path(boost::filesystem::current_path().parent_path());

	Config::Instance().SetFilename(m_Settings.m_ConfigFilename);
	Config::Instance().Load(true);
	
#ifdef HAWK_DEBUG
	if (m_Settings.m_bConsole)
	{
		ConsoleAPI::Start();
		ThreadInfoManager::RegisterThread(Thread::sc_MainThreadName, std::this_thread::get_id());
		LOG("Logger awakened...", "core", Info);
		
		m_Dispatcher = std::make_shared<Dispatcher>();
		m_ConsoleCommandManager = std::make_shared<ConsoleCommandManager>(m_Dispatcher);
		m_ConsoleCommandManager->Start();
		RegisterConsole();
	}

	ProfilerManager::Initialize(m_ConsoleCommandManager.get(), m_Dispatcher.get());
#endif
	LOG("Working directory set to: " << boost::filesystem::current_path(), "core", Info);

	Random::Initialize(Config::Instance().Get<UINT32>("dev.randSeed", 0));

	AddModules();	
	WindowManager::Initialize(m_EventRouter);

	LOG("***** Hawk core initialized *****", "core", Info);
}

ThreadID Core::CreateModuleThread(const std::string& p_Name)
{
	THROW_IF(StringUtil::AreEqual(p_Name, Thread::sc_MainThreadName), "Cannot create a module thread with the same name as the core thread");
	THROW_IF(ModuleThreadExists(p_Name), "A thread has already been created with name: " << p_Name);

	std::unique_ptr<ModuleThread> l_ModuleThread = std::make_unique<ModuleThread>(p_Name);
	ThreadID l_ThreadID = l_ModuleThread->GetThreadID();
	m_ModuleThreads.push_back(std::move(l_ModuleThread));

	LOG("Thread created. Name=" << p_Name << " ID=" << l_ThreadID, "core", Debug);
	return l_ThreadID;
}

void Core::OpenWindow(HINSTANCE p_hInstance, const std::string& p_Name)
{
	WindowManager::Open(p_hInstance, p_Name);
}

void Core::RemoveModule(ModuleID p_ID)
{
	auto l_Itr = std::find_if(m_CoreModules.begin(), m_CoreModules.end(), [p_ID](const std::unique_ptr<Module>& p_Module) { return p_Module->GetID() == p_ID; });
	if (l_Itr != m_CoreModules.end())
	{
		m_CoreModules.erase(l_Itr);
		LOG("Removed core module with ID=" << p_ID, "core", Debug);
		return;
	}

	for (auto& l_ModuleThread : m_ModuleThreads)
	{
		if (l_ModuleThread->TryRemove(p_ID))
		{
			LOG("Removed module with ID=" << p_ID << " from thread=" << l_ModuleThread->GetThreadID(), "core", Debug);
			return;
		}
	}
	LOG("Failed to remove module with ID=" << p_ID << " (not found)", "core", Error);
}

void Core::SetPaused(ModuleID p_ID, bool p_bPaused)
{
	Module* l_Module = nullptr;
	if (TryGetModule(p_ID, &l_Module))
	{
		l_Module->SetPaused(p_bPaused);
	}
	else
	{
		LOG("Failed to change pause state for module with ID=" << p_ID << " (not found)", "core", Error);
	}
}

void Core::Run()
{
	WindowManager::Open(m_Settings.m_hInstance, m_Settings.m_WindowName);
	InitializeModules();
	StartModules();

	m_PrevFrameStartTime.SetToNow();

	while (!Logger::FatalFlagSet())
	{
		Profiler l_Profiler("CoreThreadUpdate");
		l_Profiler.Start();

		Time l_CurrTime;
		l_CurrTime.SetToNow();
		Duration l_Delta(l_CurrTime - m_PrevFrameStartTime);
		l_Delta = std::min(l_Delta, ModuleThread::sc_MaxFrameDuration);

		Config::Instance().Update();
		if (!WindowManager::Update())
		{
			LOG("MainWindow signalled WM_QUIT", "core", Info);
			break;
		}
		m_Dispatcher->Execute();

		for (auto& l_Module : m_CoreModules)
		{
			l_Module->_Update(l_Delta);
		}
		Thread::Sleep();
		m_PrevFrameStartTime = l_CurrTime;
	}
	StopModules();
}

void Core::AddModules()
{
	if (m_Settings.m_bRenderingModule)
	{
		AddModule<Gfx::RenderingModule>(Thread::sc_MainThreadID);
	}
	AddModule<SceneManager>(CreateModuleThread("scene"));
}

void Core::InitializeModules()
{
	for (auto& l_Module : m_CoreModules)
	{
		l_Module->_Initialize(std::make_unique<EventManager>(m_EventRouter), m_Dispatcher);
#if HAWK_DEBUG
		l_Module->_InitializeConsole(m_ConsoleCommandManager);
#endif
	}

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
		l_Manager.reset();
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
	for (const auto& l_Module : m_CoreModules)
	{
		if (l_Module->GetID() == p_ID)
		{
			*p_Module = l_Module.get();
			return true;
		}
	}
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
	Util_Console::Register(m_ConsoleCommandManager.get(), m_Dispatcher);
}

void Core::CmdListModules()
{
	CONSOLE_WRITE_SCOPE();

	if (!m_CoreModules.empty())
	{
		std::cout << "[" << Thread::sc_MainThreadName << " #" << Thread::sc_MainThreadID << "]\n";
		for (const auto& l_Module : m_CoreModules)
		{
			l_Module->DebugPrint();
		}
		std::cout << "\n";
	}

	for (const auto& l_ModuleThread : m_ModuleThreads)
	{
		l_ModuleThread->DebugPrint();
	}
}

#endif
	
}