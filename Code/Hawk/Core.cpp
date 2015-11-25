#include "pch.h"
#include "Core.h"
#include "Time.h"
#include "WindowManager.h"
#include "Duration.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace Hawk {

namespace { const char* c_SysName("core"); }

Core::Core(bool p_bConsole)
: m_EventRouter(std::make_shared<EventRouter>())
{
#ifdef HAWK_DEBUG
	boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
	if (p_bConsole)
	{
		THROW_IF_NOT(Logger::Initialize(), "Failed to initialize LogSystem");
	}
	Config::Instance().Load();
	LOG("Working directory set to: " << boost::filesystem::current_path(), c_SysName, Info);
#endif
	WindowManager::Initialize(m_EventRouter);
	LOG("Hawk core initialized...", c_SysName, Info);
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = m_SystemManagers.insert(SystemManagers_t::value_type(p_Name, std::make_unique<SystemManager>(p_Name))).second;
	THROW_IF_NOT(l_bInserted, "Thread with name " << p_Name << " already registered");
}

void Core::OpenWindow(HINSTANCE p_hInstance, const std::string& p_Name)
{
	try
	{
		WindowManager::Open(p_hInstance, p_Name);
	}
	catch (Exception& e)
	{
		LOG_EXCEPTION(e, c_SysName, Fatal);
	}
}

void Core::Run()
{
	InitializeSystems();
	StartSystems();

	while (Config::Instance().Get<bool>("Core.run", true) && !Logger::FatalFlagSet())
	{
		Config::Instance().Update();
		if (!WindowManager::Update())
		{
			LOG("MainWindow signalled WM_QUIT", c_SysName, Info);
			break;
		}
	}
	StopSystems();

	LOG("************* Core exit *************", c_SysName, Info);
	if (Logger::FatalFlagSet())
	{
		FATAL("Core exit due to critical error (see above)", c_SysName);
	}
	std::this_thread::sleep_for(std::chrono::minutes(1));
}

void Core::InitializeSystems()
{
	for (auto& l_Manager : m_SystemManagers)
	{
		l_Manager.second->Initialize(m_EventRouter);
	}
}

void Core::StartSystems()
{
	for (auto& l_Manager : m_SystemManagers)
	{
		l_Manager.second->Start();
	}
}

void Core::StopSystems()
{
	for (auto& l_Manager : m_SystemManagers)
	{
		l_Manager.second->Stop();
	}
}
	
}