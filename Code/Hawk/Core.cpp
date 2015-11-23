#include "pch.h"
#include "Core.h"
#include "Time.h"
#include "Duration.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <thread>
#include <chrono>

namespace Hawk {

Core::Core(bool p_bConsole)
{
#ifdef HAWK_DEBUG
	boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
	if (p_bConsole)
	{
		THROW_IF_NOT(Logger::Initialize(), "Failed to initialize LogSystem");
	}
	Config::Instance().Load();
	LOG_INFO("Working directory set to: " << boost::filesystem::current_path());
#endif
	LOG_INFO("Hawk core initialized...");
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = m_SystemManagers.insert(SystemManagers_t::value_type(p_Name, std::make_unique<SystemManager>(p_Name))).second;
	THROW_IF_NOT(l_bInserted, "Thread with name " << p_Name << " already registered");
}

void Core::Run()
{
	InitializeSystems();
	StartSystems();

	while (Config::Instance().Get<bool>("Core.run", true))
	{
		Config::Instance().Update();
	}

	LOG_INFO("************* Core exit *************");
	using namespace std::literals;
	std::this_thread::sleep_for(2s);
}

void Core::InitializeSystems()
{
	for (auto& l_Manager : m_SystemManagers)
	{
		l_Manager.second->Initialize();
	}
}

void Core::StartSystems()
{
	for (auto& l_Manager : m_SystemManagers)
	{
		l_Manager.second->Start();
	}
}
	
}