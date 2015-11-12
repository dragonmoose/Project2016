#include "pch.h"
#include "Core.h"
#include "SystemBase.h"
#include <boost/filesystem.hpp>
#include <iostream>

namespace Hawk {

Core::Core()
{
	try
	{
#ifdef HAWK_DEV
		boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
		THROW_IF_NOT(Logger::Initialize(), "Failed to initialize LogSystem");
		LOG_INFO("Working directory set to: " << boost::filesystem::current_path());
		Config::Initialize();
#endif
	}
	catch (Exception& e)
	{
		std::cout << "Could not start Core: " << e.what() << std::endl;
	}
	LOG_INFO("Hawk core initialized...")
}

Core& Core::Instance()
{
	static Core l_Instance;
	return l_Instance;
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
	
}