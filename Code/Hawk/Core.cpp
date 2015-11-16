#include "pch.h"
#include "Core.h"
#include "SystemManager.h"
#include "SystemBase.h"
#include "Time.h"
#include "Duration.h"
#include <boost/filesystem.hpp>
#include <iostream>

namespace Hawk {

namespace Core
{
	typedef std::unordered_map<std::string, std::unique_ptr<SystemManager>> ThreadedSystems_t;
	ThreadedSystems_t n_ThreadedSystems;
	SystemManager n_MainSystems;
}

HAWK_DLL_EXPORT bool Core::Initialize()
{
	try
	{
#ifdef HAWK_DEBUG
		boost::filesystem::current_path(boost::filesystem::current_path().parent_path());
		THROW_IF_NOT(Logger::Initialize(), "Failed to initialize LogSystem");
		Config::Instance().Load();
		LOG_INFO("Working directory set to: " << boost::filesystem::current_path());
#endif
	}
	catch (Exception& e)
	{
		std::cout << "Failed to initialize Core. Exception: " << e.what() << std::endl;
		return false;
	}
	RegisterThread("Main");
	LOG_INFO("Hawk core initialized...");
	return true;
}

void Core::RegisterThread(const std::string& p_Name)
{
	bool l_bInserted = n_ThreadedSystems.insert(ThreadedSystems_t::value_type(p_Name, std::make_unique<SystemManager>())).second;
	THROW_IF_NOT(l_bInserted, "Thread with name " << p_Name << " already registered");

	//m_SystemThreads[p_Name] = std::make_unique<SystemThread>();
}

void Core::AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread)
{
	if (p_Thread.empty())
	{
		n_MainSystems.AddSystem(std::move(p_System));
	}
}

void Core::Run()
{
	n_MainSystems.Prepare();

	Time l_OldTime;
	while (true)
	{
		Time l_CurrTime;
		l_CurrTime.SetToNow();
		Duration l_Delta(l_CurrTime - l_OldTime);
		l_OldTime = l_CurrTime;

		Config::Instance().Update();
		n_MainSystems.Update(l_Delta);
	}
}
	
}