#include "pch.h"
#include "Core.h"
#include "ConsoleAPI.h"
#include "ConsoleCmdManager.h"
#include "WindowManager.h"
#include "Duration.h"
#include "Time.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace Hawk {

namespace { const char* c_Name("Core"); }

Core::Core(bool p_bConsole)
: m_EventRouter(std::make_shared<EventRouter>())
{
#ifdef HAWK_DEBUG
	boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
	if (p_bConsole)
	{
		ConsoleAPI::Start();
		m_ConsoleCmdManager.Start();
	}
	Config::Instance().Load();
	LOG("Working directory set to: " << boost::filesystem::current_path(), c_Name, Info);
#endif
	WindowManager::Initialize(m_EventRouter);
	LOG("Hawk core initialized...", c_Name, Info);
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = m_ModuleManagers.insert(ModuleManagers_t::value_type(p_Name, std::make_unique<ModuleManager>(p_Name))).second;
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
		LOG_EXCEPTION(e, c_Name, Fatal);
	}
}

void Core::Run()
{
	InitializeModules();
	StartModules();

	while (Config::Instance().Get<bool>("Core.run", true) && !Logger::FatalFlagSet())
	{
		Config::Instance().Update();
		m_ConsoleCmdManager.Update();
		if (!WindowManager::Update())
		{
			LOG("MainWindow signalled WM_QUIT", c_Name, Info);
			break;
		}
	}
	StopModules();
	m_ConsoleCmdManager.Stop();

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
	for (auto& l_Manager : m_ModuleManagers)
	{
		l_Manager.second->Initialize(m_EventRouter, m_ConsoleCmdManager);
	}
}

void Core::StartModules()
{
	for (auto& l_Manager : m_ModuleManagers)
	{
		l_Manager.second->Start();
	}
}

void Core::StopModules()
{
	for (auto& l_Manager : m_ModuleManagers)
	{
		l_Manager.second->Stop();
	}
}
	
}