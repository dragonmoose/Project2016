#include "pch.h"
#include "Core.h"
#include <iostream>

namespace Hawk {

Core::Core()
{
#ifdef HAWK_DEV
	if (!LogSystem::Initialize())
	{
		std::cout << "Failed to initialize LogSystem\n";
		PostQuitMessage(-1);
	}
	LOG_INFO("Hawk core initialized...");
#endif
}

Core::~Core()
{
}

Core& Core::Get()
{
	static Core l_Instance;
	return l_Instance;
}

}