#pragma once
#include "SystemManagerThread.h"
#include "DllExport.h"
#include <memory>
#include <unordered_map>

namespace Hawk {

namespace Core
{
	HAWK_DLL_EXPORT bool Initialize();
	HAWK_DLL_EXPORT void RegisterThread(const std::string& p_Name);
	HAWK_DLL_EXPORT void AddSystem(std::unique_ptr<SystemBase> p_System, const std::string& p_Thread);
	HAWK_DLL_EXPORT void Run();
};

}