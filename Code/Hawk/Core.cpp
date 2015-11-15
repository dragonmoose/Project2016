#include "pch.h"
#include "Core.h"
#include "SystemBase.h"
#include "Time.h"
#include <boost/filesystem.hpp>
#include <iostream>

namespace Hawk {

namespace Core
{
	typedef std::unordered_map<std::string, std::unique_ptr<SystemManagerThread>> SystemManagerThreads;
	SystemManagerThreads m_SystemManagerThreads;
}

HAWK_DLL_EXPORT bool Core::Initialize()
{
	try
	{
#ifdef HAWK_DEBUG
		boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
		THROW_IF_NOT(Logger::Initialize(), "Failed to initialize LogSystem");
		LOG_INFO("Working directory set to: " << boost::filesystem::current_path());
#endif
	}
	catch (Exception& e)
	{
		std::cout << "Failed to initialize Core. Exception: " << e.what() << std::endl;
		return false;
	}
	LOG_INFO("Hawk core initialized...");
	return true;
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = m_SystemManagerThreads.insert(SystemManagerThreads::value_type(p_Name, std::make_unique<SystemManagerThread>())).second;
	THROW_IF_NOT(l_bInserted, "Thread with name " << p_Name << " already registered");

	//m_SystemThreads[p_Name] = std::make_unique<SystemThread>();
}

void Core::AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread)
{
}

void Core::Run()
{
	while (true)
	{
		Config::Instance().Update();
		LOG_DEBUG("running...");
	}
}
	
}