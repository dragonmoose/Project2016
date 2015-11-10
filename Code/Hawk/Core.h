#pragma once
#include "Macros.h"
#include "SystemThread.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

class SystemBase;

class Core final
{
public:
	HAWK_DLL_EXPORT static Core& Get();

	HAWK_DLL_EXPORT void RegisterThread(const std::string& p_Name);
	HAWK_DLL_EXPORT void AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread);

	Core(const Core&)				= delete;
	Core& operator=(const Core&)	= delete;
private:
	Core();

	typedef std::unordered_map<std::string, std::unique_ptr<SystemThread>> SystemThreads;
	SystemThreads m_SystemThreads;
};

}