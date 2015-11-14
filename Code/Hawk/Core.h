#pragma once
#include "SystemManagerThread.h"
#include "DllExport.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

class Core final
{
public:
	HAWK_DLL_EXPORT static Core& Instance();

	HAWK_DLL_EXPORT void RegisterThread(const std::string& p_Name);
	HAWK_DLL_EXPORT void AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread);
	HAWK_DLL_EXPORT void Run();

	Core(const Core&)				= delete;
	Core& operator=(const Core&)	= delete;
private:
	Core();

	typedef std::unordered_map<std::string, std::unique_ptr<SystemManagerThread>> SystemManagerThreads;
	SystemManagerThreads m_SystemManagerThreads;
};

}