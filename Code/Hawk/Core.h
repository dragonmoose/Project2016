#pragma once
#include "DllExport.h"
#include "EventRouter.h"
#include "Exception.h"
#include "SystemBase.h"
#include "SystemManager.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

class HAWK_DLL_EXPORT Core final
{
public:
	Core(bool p_bConsole = true);
	Core(const Core&) = delete;
	Core& operator=(const Core&) = delete;

	void RegisterThread(const std::string& p_Name);

	template<class T>
	void AddSystem(const std::string& p_Thread = std::string())
	{
		THROW_IF(p_Thread.empty(), "Empty thread name");
		auto l_Itr = m_SystemManagers.find(p_Thread);
		THROW_IF(l_Itr == m_SystemManagers.end(), "Thread with name " << p_Thread << " not registered");
		l_Itr->second->AddSystem(SystemBase::CreateInstance<T>());
	}
	void Run();

private:
	void InitializeSystems();
	void StartSystems();

	using SystemManagers_t = std::unordered_map<std::string, std::unique_ptr<SystemManager>>;
	SystemManagers_t m_SystemManagers;
	std::shared_ptr<EventRouter> m_EventRouter;
};

}