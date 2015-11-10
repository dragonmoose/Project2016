#include "pch.h"
#include "Core.h"
#include "SystemBase.h"
#include "Exception.h"
#include <iostream>

namespace Hawk {

Core::Core()
{
	try
	{
#ifdef HAWK_DEV
		THROW_IF_NOT(LogSystem::Initialize(), "Failed to initialize LogSystem");
#endif
	}
	catch (Exception& e)
	{
		std::cout << "Could not start Core: " << e.what() << std::endl;
	}
	LOG_INFO("Hawk core initialized...");
}

Core& Core::Get()
{
	static Core l_Core;
	return l_Core;
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = m_SystemThreads.insert(SystemThreads::value_type(p_Name, std::make_unique<SystemThread>())).second;
	THROW_IF_NOT(l_bInserted, "Thread with name " << p_Name << " already registered");

	//m_SystemThreads[p_Name] = std::make_unique<SystemThread>();
}

void Core::AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread)
{

}
	
}