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
		Logger::RegisterThreadName(Thread::MainThreadName, std::this_thread::get_id());
		m_ConsoleCommandManager->Start();
		RegisterConsole();
	}
	Config::Instance().Load();
	LOG("Working directory set to: " << boost::filesystem::current_path(), c_Name, Info);
#endif
	WindowManager::Initialize(m_EventRouter);
	LOG("Hawk core initialized...", c_Name, Info);
}

void Core::CreateModuleThread(const std::string& p_Name)
{
	ModuleThreads_t::iterator l_Itr = FindByThreadName(p_Name);
	THROW_IF_NOT(l_Itr == m_ModuleThreads.end(), "Module thread with name " << p_Name << " already created");
	m_ModuleThreads.push_back(std::make_unique<ModuleThread>(p_Name));
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

Core::ModuleThreads_t::iterator Core::FindByThreadName(const std::string& p_Name)
{
	return std::find_if(m_ModuleThreads.begin(), m_ModuleThreads.end(), [p_Name](const std::unique_ptr<ModuleThread>& p_ModuleThread)
	{
		return StringUtil::AreEqual(p_ModuleThread->GetName(), p_Name);
	});
}

#ifdef HAWK_DEBUG
void Core::RegisterConsole()
{
	m_ConsoleCommandManager->Register("module.remove", this, &Core::RemoveModule, m_Dispatcher.get());
}
#endif
	
}