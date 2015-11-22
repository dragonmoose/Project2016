#pragma once
#include "DllExport.h"
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
	void AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread = std::string());
	void Run();

private:
	void InitializeSystems();
	void StartSystems();

	typedef std::unordered_map<std::string, std::unique_ptr<SystemManager>> SystemManagers_t;
	SystemManagers_t m_SystemManagers;
};

}